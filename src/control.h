#pragma once

#include "axis.h"

// https://github.com/kachurovskiy/nanoels

extern volatile int mode; // mode of operation (ELS, multi-start ELS, asynchronous)
extern bool isOn;
extern int emergencyStop;
extern long moveStep; // thousandth of a mm
extern int starts; // number of starts in a multi-start thread
extern long dupr; // pitch, tenth of a micron per rotation
extern bool beepFlag;
extern SemaphoreHandle_t motionMutex; // controls blocks of code where variables affecting the motion loop() are changed
extern long setupIndex; // Index of automation setup step
extern bool auxForward; // True for external, false for external thread
extern long opIndex; // Index of an automation operation
extern bool opIndexAdvanceFlag; // Whether user requested to move to the next pass
extern long opSubIndex; // Sub-index of an automation operation
extern int turnPasses; // In turn mode, how many turn passes to make
extern float coneRatio; // In cone mode, how much X moves for 1 step of Z

void controlSetup(Preferences& pref);
void runControl();
void applySettings(); // Apply changes requested by the keyboard thread.
bool isPassMode(); // Whether the current mode is pass mode
void setIsOnFromTask(bool on);
long posFromSpindle(Axis* a, long s, bool respectStops); // Calculates stepper position from spindle position.
void setEmergencyStop(int kind);
void markOrigin();
void updateAsyncTimerSettings();
long spindleFromPos(Axis* a, long p); // Calculates spindle position from stepper position.
void reset();
void beep();
void setStarts(int value);
void setTurnPasses(int value);
long normalizePitch(long pitch);
void setDupr(long value);
void setModeFromTask(int value);
void setConeRatio(float value);
bool needZStops();
bool isPassMode();
int getLastSetupIndex();
long getPassModeZStart();
long getPassModeXStart();