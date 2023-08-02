#pragma once

// https://github.com/kachurovskiy/nanoels


extern unsigned long spindleEncTime; // micros() of the previous spindle update
extern unsigned long spindleEncTimeDiffBulk; // micros() between RPM_BULK spindle updates
extern long spindlePos; // Spindle position
extern long spindlePosAvg; // Spindle position accounting for encoder backlash
extern int spindlePosSync; // Non-zero if gearbox is on and a soft limit was removed while axis was on it
extern long spindlePosGlobal; // global spindle position that is unaffected by e.g. zeroing

void spindleSetup(Preferences& pref);
void zeroSpindlePos();
long spindleModulo(long value);
void processSpindlePosDelta();
void discountFullSpindleTurns();

void IRAM_ATTR spinEnc();
