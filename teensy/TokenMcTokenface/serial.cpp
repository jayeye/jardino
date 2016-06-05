#include <Arduino.h>

#include "datetime.h"

static unsigned long pctime;

static uint8_t const kBufferSize = 132;
static char lin[kBufferSize + 1];
static uint8_t bp = 0;

static void procbuf() {
  lin[bp] = '\0';
#ifdef DEBUG
  Serial.print("\n<"); Serial.print(lin); Serial.println(">");
#endif
  switch (lin[0]) {
  case 'T':
  case 't':
    pctime = String(&lin[1]).toInt();
    clock_set_time(pctime);
    break;
  }
}

void poll_serial() {
  if (!Serial.available()) {
    return;
  }
  int c = Serial.read();
  if (c > 0) {
    if (c == '\n' || c == '\r') {
      if (bp > 0) {
        procbuf();
      }
      bp = 0;
    } else if (bp < kBufferSize) {
      lin[bp] = c;
      ++bp;
    }
  }
}
