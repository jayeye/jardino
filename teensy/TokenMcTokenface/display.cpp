#include "display.h"

#include <vector>

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include <droid.h>
#include <miscmono.h>

#include "secrets.h"
#include "clockface.h"
#include "totp.h"

static const int kGaugeR = 18;
static const int kGaugeR2 = kGaugeR + 1;
static const int kGaugeX = kGaugeR + 2;
static const int kGaugeY = 237 - kGaugeR;
static const int kDegreeSteps = 3;
static int xs[360 * kDegreeSteps];
static int ys[360 * kDegreeSteps];

Display::Display(Adafruit_ILI9341* tft) :
  tft_(tft),
  modified_(true),
  refresh_otp_(false),
  v_size_(0),
  cursor_(0),
  previous_angle_(0),
  previous_otp_(" ") {
  for (int i = 0; i < 360 * kDegreeSteps; ++i) {
    float a = (float)i / (float)kDegreeSteps * 0.0174532925;
    xs[i] = (int)((float)kGaugeX + sin(a) * (float)kGaugeR2);
    ys[i] = (int)((float)kGaugeY - cos(a) * (float)kGaugeR2);
#ifdef DEBUG
    Serial.println(String(i) + " " + String(xs[i]) + " " + String(ys[i]));
#endif
  }
}

void Display::begin() {
  // TODO(ji): Figure out if this is worth fixing.
  uint32_t millis_calibrate = 0;
  volatile int last_second = second();
  const int kSamples = 2;
  for (int samples = 0; samples < kSamples;) {
    int new_second = second();
    if (new_second != last_second) {
      millis_calibrate += millis() % 1000;
      ++samples;
      last_second = new_second;
    }
  }
  millis_calibrate /= kSamples;
  millis_calibrate_ = millis_calibrate;
  tft_->setRotation(3);
  tft_->fillScreen(ILI9341_BLACK);
}

void Display::secrets(Secrets* s) {
  secrets_ = s;
  v_size_ = s->size();
}

char dbuf[128];
const GFXfont *kSmallFont = DroidSans_8;
const GFXfont *kLargeFont = DroidSans_12;
void Display::show_name(int n) {
  if (((n == -1) && (cursor_ == 0)) || ((n == 1) && (cursor_ == (v_size_ - 1)))) {
    return;
  }
  snprintf(dbuf, 127, "%d/%d %s", cursor_ + n + 1, v_size_, secrets_->name_at(cursor_ + n).c_str());
  int y = 0;
  uint16_t color = 0;
  switch (n) {
  case 1: 
    y += kSmallFont->yAdvance + kLargeFont->yAdvance;
  case -1:
    tft_->setFont(kSmallFont);
    color = ILI9341_LIGHTGREY;
    break;
  case 0:
    tft_->setFont(kLargeFont);    
    color = ILI9341_WHITE;
    y += kSmallFont->yAdvance;
  }
  jshow(dbuf, 0, y, LEFT_JUSTIFY, TOP_JUSTIFY, color, ILI9341_BLACK);
}

void Display::up() {
#ifdef DEBUG
  Serial.print("UP: ");
#endif
  if (cursor_ > 0) {
    --cursor_;
    modified_ = true;
  }
#ifdef DEBUG
  Serial.println(cursor_);
#endif
}

void Display::down() {
#ifdef DEBUG
  Serial.print("DOWN: ");
#endif
  if (cursor_ < v_size_ - 1) {
    ++cursor_;
    modified_ = true;
  }
#ifdef DEBUG
  Serial.println(cursor_);
#endif
}

//        <------------------gt_w------------------>
//   (gt_x, gt_y) 
// .-->   BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB ^
// | +    BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB |
// V      BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB |
//        BBBBBB          BBBBBBBBBBBBBBBBBBBBBBBBBB gt_h
//        BBBBBB * (x, y) BBBBBBBBBBBBBBBBBBBBBBBBBB |
//        BBBBBB          BBBBBBBBBBBBBBBBBBBBBBBBBB |
//        BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB v
//
// If cursor is at (0, 0) and we ask getTextBounds, we get back
//   gt_x, gt_y, gt_w, gt_h.
// If we:
//   setCursor(x, y)
//   fillRect(gt_x, gt_y, gt_w, gt_h, c) surrounds the text
//   printed with cursor at (x, y).
// To print with (x, y) at the UL, we need to
//   setCursor(x - gt_x, y - gt_y)
//   fillRect(x, y, gt_w, gt_h, c) surrounds the text
// To print with (x, y) at the LL, we need to
//   setCursor(x - gt_x, y - gt_y - gt_h);
//   fillRect(x, y - gt_h, gt_w, gt_h, c) surrounds the text
// To print with (x, y) at the CENTER, we need to 
//   setCursor((x - gt_x - gt_w / 2, y - gt_y - gt_h / 2);
//   fillRect(x - gt_w / 2, y - gt_h / 2, gt_w, gt_h, c) surrounds the text
void Display::jshow(const char*s,
                    int x, int y,
                    int h_justify, int v_justify,
                    uint16_t text_color, uint16_t bg_color) {
  int16_t gt_x, gt_y;
  uint16_t gt_w, gt_h;
  tft_->getTextBounds(s, 0, 0, &gt_x, &gt_y, &gt_w, &gt_h);
  int hj = gt_w * (h_justify - 1) / 2;
  int vj = gt_h * (v_justify - 1) / 2;
  tft_->fillRect(x + hj, y + vj, gt_w, gt_h, bg_color);
  tft_->setTextColor(text_color, bg_color);
  tft_->setCursor(x - gt_x + hj, y - gt_y + vj);
  tft_->print(s);
  //  tft_->drawFastHLine(x, y, 16, ILI9341_YELLOW),
  //  tft_->drawFastHLine(x - 16, y, 16, ILI9341_YELLOW),
  //  tft_->drawFastVLine(x, y, 16, ILI9341_YELLOW),
  //  tft_->drawFastVLine(x, y - 16, 16, ILI9341_YELLOW),
}

int16_t Display::width() {
  return tft_->width();
}

int16_t Display::height() {
  return tft_->height();
}

void Display::gauge() {
  int half_minute = second() % 30;
  int angle = ((half_minute) * 12 + ((millis() - millis_calibrate_) % 1000) / (1000 / 12));
  if (angle < previous_angle_) {
    refresh_otp_ = true;
    previous_angle_ = 0;
  }
  if (modified_ || refresh_otp_) {
    tft_->fillCircle(kGaugeX , kGaugeY, kGaugeR, ILI9341_GREEN);
    previous_angle_ = 0;
  }
  for (int a = previous_angle_; a < kDegreeSteps * angle; ++a) {
    tft_->drawLine(kGaugeX, kGaugeY, xs[a], ys[a], ILI9341_BLACK);
  }
  previous_angle_ = angle;
}

void Display::refresh(time_t t, bool full_refresh) {
  if (modified_ || full_refresh) {
    tft_->fillScreen(0);
    tft_->setFont(Inconsolata_Bold_48);
    if (v_size_ == 0) {
      jshow("EMPTY", width() / 2, height() / 2,
            CENTER_JUSTIFY, CENTER_JUSTIFY,
            ILI9341_RED, ILI9341_BLACK);
    } else {
      show_name(-1);
      show_name(0);
      show_name(1);
      tft_->setFont(DroidSans_12);
      jshow(secrets_->description_at(cursor_).c_str(),
            0, height() / 2 + Inconsolata_Bold_48->yAdvance / 2 + 4,
            LEFT_JUSTIFY, TOP_JUSTIFY,
            ILI9341_WHITE, ILI9341_BLACK);
    }
  }
  if (modified_ || refresh_otp_ || full_refresh) {
    String otp(totp(secrets_->secret_at(cursor_), t + secrets_->tzoffset()));
    tft_->setFont(Inconsolata_Bold_48);
    jshow(previous_otp_.c_str(),
          width() / 2, height() / 2,
          CENTER_JUSTIFY, CENTER_JUSTIFY,
          ILI9341_BLACK, ILI9341_BLACK);
    jshow(otp.c_str(),
          width() / 2, height() / 2,
          CENTER_JUSTIFY, CENTER_JUSTIFY,
          ILI9341_YELLOW, ILI9341_BLACK);
    previous_otp_ = otp;
    refresh_otp_ = false;
  }
  if (v_size_ != 0) {
    gauge();
  }
  modified_ = false;
}

void Display::clock_setup() {
  ::clock_setup();
}

void Display::clock_poll(time_t t, bool draw_seconds, bool redraw) {
  if (redraw) {
    ::clock_setup();
  }
  ::clock_poll(t, draw_seconds);
}

void Display::select() {
  modified_ = true;
  Keyboard.println(previous_otp_);
}
