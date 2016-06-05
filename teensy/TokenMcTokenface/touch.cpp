#define DEBUG

#include "touch.h"

//#include <memory>

#include <XPT2046_Touchscreen.h>
#include <SPI.h>

const uint8_t Touch::kTouchMaskCurrent  = 0x001;
const uint8_t Touch::kTouchMaskPrevious = 0b010;
const uint8_t Touch::kTouching =
    Touch::kTouchMaskPrevious | Touch::kTouchMaskCurrent;
const uint8_t Touch::kNotTouching = 0 | 0;
const uint8_t Touch::kPenDown = 0 | Touch::kTouchMaskCurrent;
const uint8_t Touch::kPenUp = Touch::kTouchMaskPrevious | 0;

const uint32_t Touch::kDefaultClickRange = 40;
const float Touch::kDefaultSlope = 2.5;

Touch::Touch(uint8_t chip_select, uint8_t touch_irq) :
  cs_(chip_select),
  irq_(touch_irq),
  click_range_(kDefaultClickRange),
  slope_(kDefaultSlope),
  was_touched_(0) {
}
    
void Touch::begin() {
//  ts_.reset(new XPT2046_Touchscreen(cs_, irq_));
  ts_ = new XPT2046_Touchscreen(cs_, irq_);
  ts_->begin();
}

void Touch::click_range(int cr) {
  click_range_ = cr;
}

void Touch::slope(float t) {
  slope_ = t;
}

Touch::PollState Touch::poll() {
  PollState retval = NO_INFO;
  uint8_t is_touched = ts_->touched() ? 0xff : 0;
  curr_point_ = ts_->getPoint();
  switch ((was_touched_ & kTouchMaskPrevious) | (is_touched & kTouchMaskCurrent)) {
  case kPenDown:
    prev_point_ = curr_point_;
#ifdef DEBUG
    Serial.println("PenDown at (" + String(curr_point_.x) + ", " + String(curr_point_.y) + ")");
#endif
  case kTouching:
    break;
  case kPenUp:
#ifdef DEBUG
    Serial.print("PenUp at (" + String(curr_point_.x) + ", " + String(curr_point_.y) + ")");
#endif
    {
      int32_t dx = (int32_t)prev_point_.x - (int32_t)curr_point_.x;
      int32_t dy = (int32_t)prev_point_.y - (int32_t)curr_point_.y;
      if (abs(dx) < click_range_ && abs(dy) < click_range_) { 
        retval = CLICK;
      } else if (abs(dx) > abs(dy)) {
        float sl = abs((float)dx / (float)dy);
        if (dx < 0) {
          retval = (sl > slope_) ? LEFT : MAYBE_LEFT;
        } else {
          retval = (sl > slope_) ? RIGHT : MAYBE_RIGHT;
        }
      } else {
        float sl = abs((float)dy / (float)dx);
        if (dy < 0) {
          retval = (sl > slope_) ? UP : MAYBE_UP;
        } else {
          retval = (sl > slope_) ? DOWN : MAYBE_DOWN;
        }
      }
#ifdef DEBUG
      Serial.println("  dx=" + String(dx) + " dy=" + String(dy));
#endif
    }
  case kNotTouching:
    break;
  }
  was_touched_ = is_touched;
  return retval;
}

int Touch::last_x() {
  return curr_point_.x;
}

int Touch::last_y() {
  return curr_point_.y;
}
