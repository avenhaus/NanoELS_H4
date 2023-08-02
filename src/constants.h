#pragma once

// https://github.com/kachurovskiy/nanoels


/********************************************\
|*  Pin Definitions
\********************************************/

/*
ESP32-S3 Pins
---+------+----+-----+-----+-----------+---------------------------
No.| GPIO | IO | RTC | ADC | Default   | Function
---+------+----+-----+-----+-----------+---------------------------
   |   0* | IO |   0 |     | Boot      | Button
   |   1  | IO |   1 | 1_0 |           | LCD_D7
   |   2  | IO |   2 | 1_1 |           | LCD_D6
   |   3* | IO |   3 | 1_2 |           | 
   |   4  | IO |   4 | 1_3 |           | Buzzer
   |   5  | IO |   5 | 1_4 |           | I2C_SCL
   |   6  | IO |   6 | 1_5 |           | I2C_SDA 
   |   7  | IO |   7 | 1_6 |           | 
   |   8  | IO |   8 | 1_7 |           | X_ENA 
   |   9  | IO |   9 | 1_8 |           | A11
   |  10  | IO |  10 | 1_9 | SPI-SS    | A12
   |  11  | IO |  11 | 2_0 | SPI-MOSI  | A13
   |  12  | IO |  12 | 2_1 | SPI-SCK   | A21
   |  13  | IO |  13 | 2_2 | SPI-MISO  | A22
   |  14  | IO |  14 | 2_3 |           | A23
   |  15  | IO |  15 | 2_4 |           |
   |  16  | IO |  16 | 2_5 |           | Z_ENA
   |  17  | IO |  17 | 2_6 |           | Z_DIR
   |  18  | IO |  18 | 2_7 |           | Z_STEP
   |  19  | IO |  19 | 2_8 | USB/JTAG  | X_DIR
   |  20  | IO |  20 | 2_9 | USB/JTAG  | X_STEP
   |  21  | IO |  21 |     |           | LCD_RS
   |  38  | IO |     |     |           | LCD_D1
   |  39  | IO |     |     |           | LCD_D2
   |  40  | IO |     |     |           | LCD_D3
   |  41  | IO |     |     | I2C_SCL   | LCD_D4
   |  42  | IO |     |     | I2C_SDA   | LCD_D5
   |  43  | IO |     |     | UART_TX0  |
   |  44  | IO |     |     | UART_RX0  |
   |  45* | IO |     |     |           |
   |  46* | IO |     |     |           |
   |  47  | IO |     |     |           | LCD_D0
   |  48  | IO |     |     |           | LCD_ENABLE
---+------+----+-----+-----+-----------+---------------------------
* Strapping pins: IO0, IO3, IO45, IO46
*/


#include "config.h"

// Version of the pref storage format, should be changed when non-backward-compatible
// changes are made to the storage logic, resulting in Preferences wipe on first start.
#define PREFERENCES_VERSION 1
#define PREF_NAMESPACE "h4"

// GCode-related constants.
const float LINEAR_INTERPOLATION_PRECISION = 0.1; // 0 < x <= 1, smaller values make for quicker G0 and G1 moves
const long GCODE_WAIT_EPSILON_STEPS = 10;

// To be incremented whenever a measurable improvement is made.
#define SOFTWARE_VERSION 6

// To be changed whenever a different PCB / encoder / stepper / ... design is used.
#define HARDWARE_VERSION 4

#define Z_ENA 16
#define Z_DIR 17
#define Z_STEP 18

#define X_ENA 8
#define X_DIR 19
#define X_STEP 20

#define PIN_BUZZ 4
#define PIN_I2C_SCL 5
#define PIN_I2C_SDA 6

#define PIN_A11 9
#define PIN_A12 10
#define PIN_A13 11

#define PIN_A21 12
#define PIN_A22 13
#define PIN_A23 14

#define PIN_LCD_RS 21
#define PIN_LCD_ENABLE 48
#define PIN_LCD_D0 47
#define PIN_LCD_D1 38
#define PIN_LCD_D2 39
#define PIN_LCD_D3 40
#define PIN_LCD_D4 41
#define PIN_LCD_D5 42
#define PIN_LCD_D6 2
#define PIN_LCD_D7 1

#define B_LEFT 57
#define B_RIGHT 37
#define B_UP 47
#define B_DOWN 67
#define B_MINUS 5
#define B_PLUS 64
#define B_ON 17
#define B_OFF 27
#define B_STOPL 7
#define B_STOPR 15
#define B_STOPU 6
#define B_STOPD 16
#define B_DISPL 14
#define B_STEP 24
#define B_SETTINGS 34
#define B_MEASURE 54
#define B_REVERSE 44
#define B_0 51
#define B_1 41
#define B_2 61
#define B_3 31
#define B_4 2
#define B_5 21
#define B_6 12
#define B_7 11
#define B_8 22
#define B_9 1
#define B_BACKSPACE 32
#define B_MODE_GEARS 42
#define B_MODE_TURN 52
#define B_MODE_FACE 62
#define B_MODE_CONE 3
#define B_MODE_CUT 13
#define B_MODE_THREAD 23
#define B_MODE_OTHER 33
#define B_X 53
#define B_Z 43
#define B_A 4
#define B_B 63

#define PREF_VERSION "v"
#define PREF_DUPR "d"
#define PREF_POS_Z "zp"
#define PREF_LEFT_STOP_Z "zls"
#define PREF_RIGHT_STOP_Z "zrs"
#define PREF_ORIGIN_POS_Z "zpo"
#define PREF_POS_GLOBAL_Z "zpg"
#define PREF_MOTOR_POS_Z "zpm"
#define PREF_DISABLED_Z "zd"
#define PREF_POS_X "xp"
#define PREF_LEFT_STOP_X "xls"
#define PREF_RIGHT_STOP_X "xrs"
#define PREF_ORIGIN_POS_X "xpo"
#define PREF_POS_GLOBAL_X "xpg"
#define PREF_MOTOR_POS_X "xpm"
#define PREF_DISABLED_X "xd"
#define PREF_POS_A1 "a1p"
#define PREF_LEFT_STOP_A1 "a1ls"
#define PREF_RIGHT_STOP_A1 "a1rs"
#define PREF_ORIGIN_POS_A1 "a1po"
#define PREF_POS_GLOBAL_A1 "a1pg"
#define PREF_MOTOR_POS_A1 "a1pm"
#define PREF_DISABLED_A1 "a1d"
#define PREF_SPINDLE_POS "sp"
#define PREF_SPINDLE_POS_AVG "spa"
#define PREF_OUT_OF_SYNC "oos"
#define PREF_SPINDLE_POS_GLOBAL "spg"
#define PREF_SHOW_ANGLE "ang"
#define PREF_SHOW_TACHO "rpm"
#define PREF_STARTS "sta"
#define PREF_MODE "mod"
#define PREF_MEASURE "mea"
#define PREF_CONE_RATIO "cr"
#define PREF_TURN_PASSES "tp"
#define PREF_MOVE_STEP "ms"
#define PREF_AUX_FORWARD "af"

#define MOVE_STEP_1 10000 // 1mm
#define MOVE_STEP_2 1000 // 0.1mm
#define MOVE_STEP_3 100 // 0.01mm

#define MOVE_STEP_IMP_1 25400 // 1/10"
#define MOVE_STEP_IMP_2 2540 // 1/100"
#define MOVE_STEP_IMP_3 254 // 1/1000" also known as 1 thou

#define MODE_NORMAL 0
#define MODE_ASYNC 2
#define MODE_CONE 3
#define MODE_TURN 4
#define MODE_FACE 5
#define MODE_CUT 6
#define MODE_THREAD 7
#define MODE_ELLIPSE 8
#define MODE_GCODE 9
#define MODE_A1 10

#define MEASURE_METRIC 0
#define MEASURE_INCH 1
#define MEASURE_TPI 2

#define ESTOP_NONE 0
#define ESTOP_KEY 1
#define ESTOP_POS 2
#define ESTOP_MARK_ORIGIN 3
#define ESTOP_ON_OFF 4
#define ESTOP_OFF_MANUAL_MOVE 5

// For MEASURE_TPI, round TPI to the nearest integer if it's within this range of it.
// E.g. 80.02tpi would be shown as 80tpi but 80.04tpi would be shown as-is.
const float TPI_ROUND_EPSILON = 0.03;

const float ENCODER_STEPS_FLOAT = ENCODER_STEPS_INT; // Convenience float version of ENCODER_STEPS_INT
const long RPM_BULK = ENCODER_STEPS_INT; // Measure RPM averaged over this number of encoder pulses
const long RPM_UPDATE_INTERVAL_MICROS = 1000000; // Don't redraw RPM more often than once per second

const long GCODE_FEED_DEFAULT_DU_SEC = 20000; // Default feed in du/sec in GCode mode
const float GCODE_FEED_MIN_DU_SEC = 167; // Minimum feed in du/sec in GCode mode - F1

#define DREAD(x) digitalRead(x)
#define DHIGH(x) digitalWrite(x, HIGH)
#define DLOW(x) digitalWrite(x, LOW)
#define DWRITE(x, y) digitalWrite(x, y)

#define DELAY(x) vTaskDelay(x / portTICK_PERIOD_MS);
