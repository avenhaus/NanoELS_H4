
// https://github.com/kachurovskiy/nanoels

#include <Arduino.h>
#include "config.h"
#include "constants.h"
#include "lcd.h"
#include "keypad.h"
#include "axis.h"
#include "spindle.h"
#include "control.h"
#include "gcode.h"

#include <Preferences.h>


void taskAttachInterrupts(void *param) {
  // Attaching interrupt on core 0 to have more time on core 1 where axes are moved.
  attachInterrupt(digitalPinToInterrupt(ENC_A), spinEnc, FALLING);
  if (PULSE_1_USE) attachInterrupt(digitalPinToInterrupt(PIN_A12), pulse1Enc, CHANGE);
  if (PULSE_2_USE) attachInterrupt(digitalPinToInterrupt(PIN_A22), pulse2Enc, CHANGE);
  vTaskDelete(NULL);
}

void setup() {
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);

  Preferences pref;
  pref.begin(PREF_NAMESPACE);
  if (pref.getInt(PREF_VERSION) != PREFERENCES_VERSION) {
    pref.clear();
    pref.putInt(PREF_VERSION, PREFERENCES_VERSION);
  }

  axisSetup(pref);
  spindleSetup(pref);
  controlSetup(pref);

  pref.end();

  Serial.begin(115200);

  keypadSetup();

  // Non-time-sensitive tasks on core 0.
  xTaskCreatePinnedToCore(taskDisplay, "taskDisplay", 10000 /* stack size */, NULL, 0 /* priority */, NULL, 0 /* core */);
  delay(100);

  xTaskCreatePinnedToCore(taskMoveZ, "taskMoveZ", 10000 /* stack size */, NULL, 0 /* priority */, NULL, 0 /* core */);
  xTaskCreatePinnedToCore(taskMoveX, "taskMoveX", 10000 /* stack size */, NULL, 0 /* priority */, NULL, 0 /* core */);
  if (a1.active) xTaskCreatePinnedToCore(taskMoveA1, "taskMoveA1", 10000 /* stack size */, NULL, 0 /* priority */, NULL, 0 /* core */);
  xTaskCreatePinnedToCore(taskAttachInterrupts, "taskAttachInterrupts", 10000 /* stack size */, NULL, 0 /* priority */, NULL, 0 /* core */);
  xTaskCreatePinnedToCore(taskGcode, "taskGcode", 10000 /* stack size */, NULL, 0 /* priority */, NULL, 0 /* core */);
}

void loop() {
  if (emergencyStop != ESTOP_NONE) {
    return;
  }
  if (xSemaphoreTake(motionMutex, 1) != pdTRUE) {
    return;
  }
  applySettings();
  runControl();
}