

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_TCA8418.h>

#include "constants.h"
#include "keypad.h"
#include "lcd.h"
#include "control.h"
#include "axis.h"


Adafruit_TCA8418 keypad;
unsigned long keypadTimeUs = 0;
unsigned long resetMillis = 0;

// Most buttons we only have "down" handling, holding them has no effect.
// Buttons with special "holding" logic have flags below.
bool buttonLeftPressed = false;
bool buttonRightPressed = false;
bool buttonUpPressed = false;
bool buttonDownPressed = false;
bool buttonOffPressed = false;
bool buttonGearsPressed = false;
bool buttonTurnPressed = false;

bool inNumpad = false;
int numpadDigits[20];
int numpadIndex = 0;

void processKeypadEvent();

void taskKeypad(void *param) {
  while (emergencyStop == ESTOP_NONE) {
    processKeypadEvent();
    taskYIELD();
  }
  vTaskDelete(NULL);
}

void keypadSetup() {
     if (!Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL)) {
    Serial.println("I2C initialization failed");
  } else if (!keypad.begin(TCA8418_DEFAULT_ADDR, &Wire)) {
    Serial.println("TCA8418 key controller not found");
  } else {
    keypad.matrix(7, 7);
    keypad.flush();
  }

  delay(100);
  if (keypad.available()) {
    setEmergencyStop(ESTOP_KEY);
    return;
  } else {
    // Non-time-sensitive tasks on core 0.
    xTaskCreatePinnedToCore(taskKeypad, "taskKeypad", 10000 /* stack size */, NULL, 0 /* priority */, NULL, 0 /* core */);
  }
}

void buttonOffRelease() {
  if (millis() - resetMillis > 3000) {
    reset();
    splashScreen = true;
  }
}


void buttonOnOffPress(bool on) {
  resetMillis = millis();
  bool missingZStops = needZStops() && (z.leftStop == LONG_MAX || z.rightStop == LONG_MIN);
  if (on && isPassMode() && (missingZStops || x.leftStop == LONG_MAX || x.rightStop == LONG_MIN)) {
    beep();
  } else if (!isOn && on && setupIndex < getLastSetupIndex()) {
    // Move to the next setup step.
    setupIndex++;
  } else if (isOn && on && (mode == MODE_TURN || mode == MODE_FACE || mode == MODE_THREAD)) {
    // Move to the next pass.
    opIndexAdvanceFlag = true;
  } else if (!on && (z.movingManually || x.movingManually || x.movingManually)) {
    setEmergencyStop(ESTOP_OFF_MANUAL_MOVE);
  } else {
    setIsOnFromTask(on);
  }
}

void buttonPlusMinusPress(bool plus) {
  // Mutex is aquired in setDupr() and setStarts().
  bool minus = !plus;
  if (mode == MODE_THREAD && setupIndex == 2) {
    if (minus && starts > 2) {
      setStarts(starts - 1);
    } else if (plus && starts < STARTS_MAX) {
      setStarts(starts + 1);
    }
  } else if (isPassMode() && setupIndex == 1 && getNumpadResult() == 0) {
    if (minus && turnPasses > 1) {
      setTurnPasses(turnPasses - 1);
    } else if (plus && turnPasses < PASSES_MAX) {
      setTurnPasses(turnPasses + 1);
    }
  } else if (measure != MEASURE_TPI) {
    int delta = measure == MEASURE_METRIC ? MOVE_STEP_3 : MOVE_STEP_IMP_3;
    // Switching between mm/inch/tpi often results in getting non-0 3rd and 4th
    // precision points that can't be easily controlled. Remove them.
    long normalizedDupr = normalizePitch(dupr);
    if (minus && dupr > -DUPR_MAX) {
      setDupr(max(-DUPR_MAX, normalizedDupr - delta));
    } else if (plus && dupr < DUPR_MAX) {
      setDupr(min(DUPR_MAX, normalizedDupr + delta));
    }
  } else { // TPI
    if (dupr == 0) {
      setDupr(plus ? 1 : -1);
    } else {
      long currentTpi = round(254000.0 / dupr);
      long tpi = currentTpi + (plus ? 1 : -1);
      long newDupr = tpi == 0 ? (plus ? DUPR_MAX : -DUPR_MAX) : round(254000.0 / tpi);
      // Happens for small pitches like 0.01mm.
      if (newDupr == dupr) {
        newDupr += plus ? -1 : 1;
      }
      if (newDupr != dupr && newDupr < DUPR_MAX && newDupr > -DUPR_MAX) {
        setDupr(newDupr);
      }
    }
  }
}

void buttonLeftStopPress(Axis* a) {
  setLeftStop(a, a->leftStop == LONG_MAX ? a->pos : LONG_MAX);
}

void buttonRightStopPress(Axis* a) {
  setRightStop(a, a->rightStop == LONG_MIN ? a->pos : LONG_MIN);
}

void buttonDisplayPress() {
  if (!showAngle && !showTacho) {
    showAngle = true;
  } else if (showAngle) {
    showAngle = false;
    showTacho = true;
  } else {
    showTacho = false;
  }
}

void buttonMoveStepPress() {
  if (measure == MEASURE_METRIC) {
    if (moveStep == MOVE_STEP_1) {
      moveStep = MOVE_STEP_2;
    } else if (moveStep == MOVE_STEP_2) {
      moveStep = MOVE_STEP_3;
    } else {
      moveStep = MOVE_STEP_1;
    }
  } else {
    if (moveStep == MOVE_STEP_IMP_1) {
      moveStep = MOVE_STEP_IMP_2;
    } else if (moveStep == MOVE_STEP_IMP_2) {
      moveStep = MOVE_STEP_IMP_3;
    } else {
      moveStep = MOVE_STEP_IMP_1;
    }
  }
}

void buttonModePress() {
  if (mode == MODE_NORMAL) {
    setModeFromTask(ACTIVE_A1 ? MODE_A1 : MODE_ELLIPSE);
  } else if (mode == MODE_A1) {
    setModeFromTask(MODE_ELLIPSE);
  } else if (mode == MODE_ELLIPSE) {
    setModeFromTask(MODE_GCODE);
  } else if (mode == MODE_GCODE) {
    setModeFromTask(MODE_ASYNC);
  } else {
    setModeFromTask(MODE_NORMAL);
  }
}

void buttonMeasurePress() {
  if (measure == MEASURE_METRIC) {
    setMeasure(MEASURE_INCH);
  } else if (measure == MEASURE_INCH) {
    setMeasure(MEASURE_TPI);
  } else {
    setMeasure(MEASURE_METRIC);
  }
}

void buttonReversePress() {
  setDupr(-dupr);
}

void numpadPress(int digit) {
  if (!inNumpad) {
    numpadIndex = 0;
  }
  numpadDigits[numpadIndex] = digit;
  if (numpadIndex < 7) {
    numpadIndex++;
  } else {
    numpadIndex = 0;
  }
}

void numpadBackspace() {
  if (inNumpad && numpadIndex > 0) {
    numpadIndex--;
  }
}

void resetNumpad() {
  numpadIndex = 0;
}

long getNumpadResult() {
  long result = 0;
  for (int i = 0; i < numpadIndex; i++) {
    result += numpadDigits[i] * pow(10, numpadIndex - 1 - i);
  }
  return result;
}

void numpadPlusMinus(bool plus) {
  if (numpadDigits[numpadIndex - 1] < 9 && plus) {
    numpadDigits[numpadIndex - 1]++;
  } else if (numpadDigits[numpadIndex - 1] > 1 && !plus) {
    numpadDigits[numpadIndex - 1]--;
  }
  // TODO: implement going over 9 and below 1.
}

long numpadToDeciMicrons() {
  long result = getNumpadResult();
  if (result == 0) {
    return 0;
  }
  if (measure == MEASURE_INCH) {
    result = result * 254;
  } else if (measure == MEASURE_TPI) {
    result = round(254000.0 / result);
  } else { // Metric
    result = result * 10;
  }
  return result;
}

float numpadToConeRatio() {
  return getNumpadResult() / 100000.0;
}

bool processNumpadResult(int keyCode) {
  long newDu = numpadToDeciMicrons();
  float newConeRatio = numpadToConeRatio();
  long numpadResult = getNumpadResult();
  resetNumpad();
  // Ignore numpad input unless confirmed with ON.
  if (keyCode == B_ON) {
    if (isPassMode() && setupIndex == 1) {
      setTurnPasses(int(min(PASSES_MAX, numpadResult)));
      setupIndex++;
    } else if (mode == MODE_CONE && setupIndex == 1) {
      setConeRatio(newConeRatio);
      setupIndex++;
    } else {
      if (abs(newDu) <= DUPR_MAX) {
        setDupr(newDu);
      }
    }
    // Don't use this ON press for starting the motion.
    return true;
  }

  // Shared piece for stops and moves.
  Axis* a = (keyCode == B_STOPL || keyCode == B_STOPR || keyCode == B_LEFT || keyCode == B_RIGHT || keyCode == B_Z) ? &z : &x;
  int sign = ((keyCode == B_STOPL || keyCode == B_STOPU || keyCode == B_LEFT || keyCode == B_UP || keyCode == B_Z || keyCode == B_X || keyCode == B_A) ? 1 : -1);
  if (mode == MODE_A1 && (keyCode == B_MODE_GEARS || keyCode == B_MODE_TURN || keyCode == B_MODE_FACE || keyCode == B_MODE_CONE || keyCode == B_MODE_THREAD)) {
    a = &a1;
    sign = (keyCode == B_MODE_GEARS || keyCode == B_MODE_FACE) ? -1 : 1;
  }
  long pos = a->pos + (a->rotational ? numpadResult * 10 : newDu) / a->screwPitch * a->motorSteps * sign;

  // Potentially assign a new value to a limit. Treat newDu as a relative distance from current position.
  if (keyCode == B_STOPL) {
    setLeftStop(&z, pos);
    return true;
  } else if (keyCode == B_STOPR) {
    setRightStop(&z, pos);
    return true;
  } else if (keyCode == B_STOPU) {
    setLeftStop(&x, pos);
    return true;
  } else if (keyCode == B_STOPD) {
    setRightStop(&x, pos);
    return true;
  } else if (mode == MODE_A1) {
    if (keyCode == B_MODE_CONE) {
      setLeftStop(&a1, pos);
      return true;
    } else if (keyCode == B_MODE_FACE) {
      setRightStop(&a1, pos);
      return true;
    }
  }

  // Potentially move by newDu in the given direction.
  // We don't support precision manual moves when ON yet. Can't stay in the thread for most modes.
  if (!isOn && (keyCode == B_LEFT || keyCode == B_RIGHT || keyCode == B_UP || keyCode == B_DOWN || (mode == MODE_A1 && (keyCode == B_MODE_GEARS || keyCode == B_MODE_TURN)))) {
    if (pos < a->rightStop) {
      pos = a->rightStop;
      beep();
    } else if (pos > a->leftStop) {
      pos = a->leftStop;
      beep();
    } else if (abs(pos - a->pos) > a->estopSteps) {
      beep();
      return true;
    }
    a->speedMax = a->speedManualMove;
    stepToFinal(a, pos);
    return true;
  }

  // Set axis 0 newDu ahead.
  if (keyCode == B_Z || keyCode == B_X || (mode == MODE_A1 && keyCode == B_MODE_THREAD)) {
    a->originPos = -pos;
    return true;
  }

  // Set X axis 0 from diameter.
  if (keyCode == B_A) {
    a->originPos = -pos / 2;
    return true;
  }

  if (keyCode == B_STEP) {
    if (newDu > 0) {
      moveStep = newDu;
    } else {
      beep();
    }
    return true;
  }

  return false;
}

bool processNumpad(int keyCode) {
  if (keyCode == B_0) {
    numpadPress(0);
    inNumpad = true;
  } else if (keyCode == B_1) {
    numpadPress(1);
    inNumpad = true;
  } else if (keyCode == B_2) {
    numpadPress(2);
    inNumpad = true;
  } else if (keyCode == B_3) {
    numpadPress(3);
    inNumpad = true;
  } else if (keyCode == B_4) {
    numpadPress(4);
    inNumpad = true;
  } else if (keyCode == B_5) {
    numpadPress(5);
    inNumpad = true;
  } else if (keyCode == B_6) {
    numpadPress(6);
    inNumpad = true;
  } else if (keyCode == B_7) {
    numpadPress(7);
    inNumpad = true;
  } else if (keyCode == B_8) {
    numpadPress(8);
    inNumpad = true;
  } else if (keyCode == B_9) {
    numpadPress(9);
    inNumpad = true;
  } else if (keyCode == B_BACKSPACE) {
    numpadBackspace();
    inNumpad = true;
  } else if (inNumpad && (keyCode == B_PLUS || keyCode == B_MINUS)) {
    numpadPlusMinus(keyCode == B_PLUS);
    return true;
  } else if (inNumpad) {
    inNumpad = false;
    return processNumpadResult(keyCode);
  }
  return inNumpad;
}

void processKeypadEvent() {
  if (keypad.available() == 0) return;
  int event = keypad.getEvent();
  int keyCode = event;
  bitWrite(keyCode, 7, 0);
  bool isPress = bitRead(event, 7) == 1; // 1 - press, 0 - release
  keypadTimeUs = micros();

  // Off button always gets handled.
  if (keyCode == B_OFF) {
    buttonOffPressed = isPress;
    isPress ? buttonOnOffPress(false) : buttonOffRelease();
  }

  if (mode == MODE_GCODE && isOn) {
    // Not allowed to interfere other than turn off.
    if (isPress && keyCode != B_OFF) beep();
    return;
  }

  // Releases don't matter in numpad but it has to run before LRUD since it might handle those keys.
  if (isPress && processNumpad(keyCode)) {
    return;
  }

  // Setup wizard navigation.
  if (isPress && setupIndex == 2 && (keyCode == B_LEFT || keyCode == B_RIGHT)) {
    auxForward = !auxForward;
  } else if (keyCode == B_LEFT) { // Make sure isPress=false propagates to motion flags.
    buttonLeftPressed = isPress;
  } else if (keyCode == B_RIGHT) {
    buttonRightPressed = isPress;
  } else if (keyCode == B_UP) {
    buttonUpPressed = isPress;
  } else if (keyCode == B_DOWN) {
    buttonDownPressed = isPress;
  } else if (keyCode == B_MODE_GEARS) {
    buttonGearsPressed = isPress;
  } else if (keyCode == B_MODE_TURN) {
    buttonTurnPressed = isPress;
  }

  // For all other keys we have no "release" logic.
  if (!isPress) {
    return;
  }

  // Rest of the buttons.
  if (keyCode == B_PLUS) {
    buttonPlusMinusPress(true);
  } else if (keyCode == B_MINUS) {
    buttonPlusMinusPress(false);
  } else if (keyCode == B_ON) {
    buttonOnOffPress(true);
  } else if (keyCode == B_STOPL) {
    buttonLeftStopPress(&z);
  } else if (keyCode == B_STOPR) {
    buttonRightStopPress(&z);
  } else if (keyCode == B_STOPU) {
    buttonLeftStopPress(&x);
  } else if (keyCode == B_STOPD) {
    buttonRightStopPress(&x);
  } else if (keyCode == B_MODE_OTHER) {
    buttonModePress();
  } else if (keyCode == B_DISPL) {
    buttonDisplayPress();
  } else if (keyCode == B_X) {
    markAxis0(&x);
  } else if (keyCode == B_Z) {
    markAxis0(&z);
  } else if (keyCode == B_A) {
    x.disabled = !x.disabled;
    updateEnable(&x);
  } else if (keyCode == B_B) {
    z.disabled = !z.disabled;
    updateEnable(&z);
  } else if (keyCode == B_STEP) {
    buttonMoveStepPress();
  } else if (keyCode == B_SETTINGS) {
    // TODO.
  } else if (keyCode == B_REVERSE) {
    buttonReversePress();
  } else if (keyCode == B_MEASURE) {
    buttonMeasurePress();
  } else if (keyCode == B_MODE_GEARS && mode != MODE_A1) {
    setModeFromTask(MODE_NORMAL);
  } else if (keyCode == B_MODE_TURN && mode != MODE_A1) {
    setModeFromTask(MODE_TURN);
  } else if (keyCode == B_MODE_FACE) {
    mode == MODE_A1 ? buttonRightStopPress(&a1) : setModeFromTask(MODE_FACE);
  } else if (keyCode == B_MODE_CONE) {
    mode == MODE_A1 ? buttonLeftStopPress(&a1) : setModeFromTask(MODE_CONE);
  } else if (keyCode == B_MODE_CUT) {
    if (mode == MODE_A1) {
      a1.disabled = !a1.disabled;
      updateEnable(&a1);
    } else {
      setModeFromTask(MODE_CUT);
    }
  } else if (keyCode == B_MODE_THREAD) {
    mode == MODE_A1 || (mode == MODE_GCODE && ACTIVE_A1) ? markAxis0(&a1) : setModeFromTask(MODE_THREAD);
  }
}

