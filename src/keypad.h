#pragma once

// https://github.com/kachurovskiy/nanoels

extern bool buttonLeftPressed;
extern bool buttonRightPressed;
extern bool buttonUpPressed;
extern bool buttonDownPressed;
extern bool buttonOffPressed;
extern bool buttonGearsPressed;
extern bool buttonTurnPressed;
extern bool inNumpad;
extern unsigned long keypadTimeUs;

void keypadSetup();
long getNumpadResult();
float numpadToConeRatio();
long numpadToDeciMicrons();