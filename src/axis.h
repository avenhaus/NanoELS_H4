#pragma once

// https://github.com/kachurovskiy/nanoels

#include <Arduino.h>

struct Axis {
  SemaphoreHandle_t mutex;

  char name;
  bool active;
  bool rotational;
  float motorSteps; // motor steps per revolution of the axis
  float screwPitch; // lead screw pitch in deci-microns (10^-7 of a meter)

  long pos; // relative position of the tool in stepper motor steps
  long savedPos; // value saved in Preferences
  float fractionalPos; // fractional distance in steps that we meant to travel but couldn't
  long originPos; // relative position of the stepper motor to origin, in steps
  long savedOriginPos; // originPos saved in Preferences
  long posGlobal; // global position of the motor in steps
  long savedPosGlobal; // posGlobal saved in Preferences
  int pendingPos; // steps of the stepper motor that we should make as soon as possible
  long motorPos; // position of the motor in stepper motor steps, same as pos unless moving back, then differs by backlashSteps
  long savedMotorPos; // motorPos saved in Preferences
  bool continuous; // whether current movement is expected to continue until an unknown position

  long leftStop; // left stop value of pos
  long savedLeftStop; // value saved in Preferences
  long nextLeftStop; // left stop value that should be applied asap
  bool nextLeftStopFlag; // whether nextLeftStop required attention

  long rightStop; // right stop value of pos
  long savedRightStop; // value saved in Preferences
  long nextRightStop; // right stop value that should be applied asap
  bool nextRightStopFlag; // whether nextRightStop requires attention

  long speed; // motor speed in steps / second
  long speedStart; // Initial speed of a motor, steps / second.
  long speedMax; // To limit max speed e.g. for manual moves
  long speedManualMove; // Maximum speed of a motor during manual move, steps / second.
  long acceleration; // Acceleration of a motor, steps / second ^ 2.
  long decelerateSteps; // Number of steps before the end position the deceleration should start.

  bool direction; // To reset speed when direction changes.
  bool directionInitialized;
  unsigned long stepStartUs;
  int stepperEnableCounter;
  bool disabled;
  bool savedDisabled;

  bool invertStepper; // change (true/false) if the carriage moves e.g. "left" when you press "right".
  bool needsRest; // set to false for closed-loop drivers, true for open-loop.
  bool movingManually; // whether stepper is being moved by left/right buttons
  long estopSteps; // amount of steps to exceed machine limits
  long backlashSteps; // amount of steps in reverse direction to re-engage the carriage
  long gcodeRelativePos; // absolute position in steps that relative GCode refers to

  int ena; // Enable pin of this motor
  int dir; // Direction pin of this motor
  int step; // Step pin of this motor
};

extern Axis z;
extern Axis x;
extern Axis a1;

void axisSetup(Preferences& pref);

void initAxis(Axis* a, char name, bool active, bool rotational, float motorSteps, float screwPitch, long speedStart, long speedManualMove,
    long acceleration, bool invertStepper, bool needsRest, long maxTravelMm, long backlashDu, int ena, int dir, int step);

void taskMoveX(void *param);
void taskMoveZ(void *param);
void taskMoveA1(void *param);

void moveAxis(Axis* a);
bool stepperIsRunning(Axis* a);
long stepsToDu(Axis* a, long steps);
bool stepToContinuous(Axis* a, long newPos);
bool stepToFinal(Axis* a, long newPos);
void stepperEnable(Axis* a, bool value);
long getAxisPosDu(Axis* a);
long getAxisStopDiffDu(Axis* a);
void markAxisOrigin(Axis* a);
void markAxis0(Axis* a);
Axis* getPitchAxis();
void setDir(Axis* a, bool dir);
void applyLeftStop(Axis* a);
void applyRightStop(Axis* a);
void updateEnable(Axis* a);
void setLeftStop(Axis* a, long value);
void setRightStop(Axis* a, long value);

void IRAM_ATTR pulse1Enc();
void IRAM_ATTR pulse2Enc();