// Author: John Ioannidis <ji@tla.org>
// License: Beer
//
// Pin assignments for my particular board; Teensy3.2 with the
// stripboard layout of toptteensy.per.

#include "pin_assignments.h"

#include <Arduino.h>

//         Teensy 3.2
//       GND       Vin
//         0       AGND
//         1       3V3
// d CS    2       23 RR
//   RS    3       22 RL
// d D/C   4       21
//         5       20
//   SW2   6       19
//         7       18
//   SW1   8       17
// t IRQ   9       16
// t CS   10       15 SW4
//   MOSI 11       14 SW3
//   MISO 12       13 SCLK

const uint8_t kTftCs = 2;
const uint8_t kTftDc = 4;
const uint8_t kTouchCs = 10;
const uint8_t kTouchIrq = 9;
const uint8_t kSdCs = 20;
const uint8_t kRotaryLeft = 22;
const uint8_t kRotaryRight = 23;
const uint8_t kRotarySelect = 3;
const uint8_t kSw1 = 8;
const uint8_t kSw2 = 6;
const uint8_t kSw3 = 14;
const uint8_t kSw4 = 15;

