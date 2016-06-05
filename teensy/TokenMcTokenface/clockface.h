#ifndef _CLOCKFACE_H_
#define _CLOCKFACE_H_
#include <Time.h>

extern void clock_setup();
extern void clock_poll(time_t t, bool draw_seconds);
#endif
