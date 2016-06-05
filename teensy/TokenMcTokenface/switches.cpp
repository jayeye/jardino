#include "pin_assignments.h"
#include "switches.h"

#include <Arduino.h>

Switches::Switches() {
}

void Switches::begin() {
  pinMode(kSw1, INPUT_PULLUP);
  pinMode(kSw2, INPUT_PULLUP);
  pinMode(kSw3, INPUT_PULLUP);
  pinMode(kSw4, INPUT_PULLUP);
  pinMode(kRotarySelect, INPUT_PULLUP);
  pinMode(kRotaryLeft, INPUT_PULLUP);
  pinMode(kRotaryRight, INPUT_PULLUP);
}

void Switches::poll() {
}
