#include "datetime.h"

#include <TimeLib.h>

static time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

void clock_begin() {
  setSyncProvider(getTeensy3Time);
}

void clock_set_time(time_t t) {
  setTime(t);
}
