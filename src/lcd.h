#pragma once

// https://github.com/kachurovskiy/nanoels

void lcdSetup(Preferences& pref);
void updateDisplay();
void setMeasure(int value);
int getApproxRpm();
void taskDisplay(void *param);

extern bool splashScreen;
extern int measure; // Whether to show distances in inches
extern bool showAngle; // Whether to show 0-359 spindle angle on screen
extern bool showTacho; // Whether to show spindle RPM on screen