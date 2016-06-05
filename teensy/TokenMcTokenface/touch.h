#ifndef _TOUCH_H_
#define _TOUCH_H_

//#include <memory>

#include <XPT2046_Touchscreen.h>

class Touch {
public:
  enum PollState {
    NO_INFO,
    CLICK,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    MAYBE_LEFT,
    MAYBE_RIGHT,
    MAYBE_UP,
    MAYBE_DOWN,
  };
  Touch(uint8_t chip_select, uint8_t touch_irq);
  void begin();
  PollState poll();
  void click_range(int cr);
  void slope(float t);
  int last_x();
  int last_y();
private:
  const uint8_t cs_;
  const uint8_t irq_;
  int click_range_;
  float slope_;
  XPT2046_Touchscreen* ts_;
//  std::unique_ptr<XPT2046_Touchscreen> ts_;
  static const uint8_t kTouchMaskCurrent;
  static const uint8_t kTouchMaskPrevious;
  static const uint8_t kTouching;
  static const uint8_t kNotTouching;
  static const uint8_t kPenDown;
  static const uint8_t kPenUp;
  static const uint32_t kDefaultClickRange;
  static const float kDefaultSlope;
  TS_Point prev_point_;
  TS_Point curr_point_;
  uint8_t was_touched_;
};
#endif
