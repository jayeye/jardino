#ifndef _DISPLAY_H_
#define _DISPLAY_H_

//#include <vector>

#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <Time.h>

#include "clockface.h"
#include "secrets.h"


class Display {
 public:
  enum justification {
    LEFT_JUSTIFY = 1,
    CENTER_JUSTIFY = 0,
    RIGHT_JUSTIFY = -1,
    BOTTOM_JUSTIFY = -1,
    TOP_JUSTIFY = 1,
  };
  Display(Adafruit_ILI9341* tft);
  void begin();
  void label();
  void epoch(uint32_t e);
  void refresh(time_t t, bool full_refresh);
  void up();
  void down();
  void secrets(Secrets* s);
  void clock_setup();
  void clock_poll(time_t t, bool draw_seconds, bool redraw);
  void select();
  void jshow(const char* s, int x, int y, int h_justify, int v_justify, uint16_t text_color, uint16_t bg_color);
  int16_t width();
  int16_t height();
private:
  Adafruit_ILI9341* tft_;
  bool modified_;
  bool refresh_otp_;
  Secrets* secrets_;
  size_t v_size_;
  size_t cursor_;
  uint32_t epoch_;
  uint32_t millis_calibrate_;
  int previous_angle_;
  String previous_otp_;
  void show_name(int n);
  void gauge();
};

#endif

