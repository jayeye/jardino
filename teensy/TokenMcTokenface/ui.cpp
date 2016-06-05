#define DEBUG

// Author: John Ioannidis <ji@tla.org>
// License: Beer

#include "ui.h"

#include <vector>

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Encoder.h>
#include <SD.h>
#include <Time.h>

#include "datetime.h"
#include "pin_assignments.h"
#include "touch.h"

extern Adafruit_ILI9341* tft;
static String empty("");

UI::UI(Menu* top) :
  top_menu_(top) {
  UI();
}

UI::UI() :
  showing_clock_(true),
  last_clock_millis_(millis()) {
}

String readfile(String f) {
  File fd = SD.open(f.c_str());
  if (!fd) {
    return empty;
  }
  size_t len = fd.size();
  String retval;
  retval.reserve(len);
  for (size_t i = 0; i < len; ++i) {
    retval += (char)fd.read();
  }
  fd.close();
  return retval;
}

void UI::begin(Display* d, Touch* t) {
  d_ = d;
  t_ = t;
  enc_ = new Encoder(kRotaryLeft, kRotaryRight);
  enc_position_ = enc_->read();
  pinMode(kRotarySelect, INPUT_PULLUP);
  select_ = 1;

  if (!SD.begin(kSdCs)) {
#ifdef DEBUG
    Serial.println("initialization failed!");
#endif
    tft->println("SD init fail");
    return;
  }

  Secrets* secrets = new Secrets();
  String secrets_dir = "/secrets/";
  String tz = readfile(secrets_dir + "tz");
  if (tz.length() != 0) {
    secrets->tzoffset(tz.toInt());
  }
  File root = SD.open(secrets_dir.c_str());
  for (;;) {
    File entry = root.openNextFile();
    if (!entry) {
      break;
    }
    if (entry.isDirectory()) {
      String one_secret_dir = secrets_dir + entry.name() + "/";
      String name = readfile(one_secret_dir + "n");
      String description = readfile(one_secret_dir + "d");
      String secret =  readfile(one_secret_dir + "s");
      if (name.length() == 0 || secret.length() == 0) {
        continue;
      }
      secrets->add(name, description, secret);
    }
  }
  d_->secrets(secrets);
}

// When idle, the display is showing a clock and the date. 
// Any movement of the dial brings up the TOTP screen.
// Movement events have to be separated by kDebounce ms.
static uint32_t kDebounce = 50;
static int32_t kRotationTolerance = 2;
static int32_t kIdleTimeout = 60000;
void UI::poll(time_t t) {
  // First, refresh whichever state we are in. 
  // If showing clock, move the hands
  uint32_t polled_millis = millis();
  if (showing_clock_) {
    if (polled_millis - last_clock_millis_ >= 1000) {
      last_clock_millis_ = polled_millis;
      d_->clock_poll(t, true, false);
    }
  } else {
    d_->refresh(t, false);
  }
  /// XXX
  if (polled_millis - last_touch_millis_ < kDebounce) {
    return;
  }
  bool selected = false;
  bool moved = false;
  bool moved_up = false;
  uint8_t sel = digitalRead(kRotarySelect);
  int32_t pos = enc_->read();
  //  tft->setFont(NULL);
  //  tft->setCursor(0, 0);
  //  tft->println(sel);
  //  tft->println(pos);
  //  tft->println(enc_position_);
  delay(1);
  if (sel == 0 && select_ == 1) {
    // falling edge!
    selected = true;
  }
  select_ = sel;
  if (pos < (enc_position_ - kRotationTolerance)) {
    moved = true;
    moved_up = false;
  }
  if (pos > (enc_position_ + kRotationTolerance)) {
    moved = true;
    moved_up = true;
  }
  enc_position_ = pos;
  switch (t_->poll()) {
  case Touch::NO_INFO:
    break;
  case Touch::CLICK:
#ifdef DEBUG
    Serial.println("T_CLICK");
#endif
    if (t_->last_y() > 120) {
      selected = true;
    }
    break;
  case Touch::LEFT:
#ifdef DEBUG
    Serial.println("T_LEFT");
#endif
    moved = true;
    moved_up = true;
    break;
  case Touch::RIGHT:
#ifdef DEBUG
    Serial.println("T_RIGHT");
#endif
    moved = true;
    moved_up = false;
    break;
  case Touch::UP:
#ifdef DEBUG
    Serial.println("T_UP");
#endif
    moved = true;
    moved_up = false; // the cursor moves down!
    break;
  case Touch::DOWN:
#ifdef DEBUG
    Serial.println("T_DOWN");
#endif
    moved = true;
    moved_up = true; // the cursor moves up!
    break;
  case Touch::MAYBE_LEFT:
#ifdef DEBUG
    Serial.println("MAYBE_LEFT");
#endif
    break;
  case Touch::MAYBE_RIGHT:
#ifdef DEBUG
    Serial.println("MAYBE_RIGHT");
#endif
    break;
  case Touch::MAYBE_UP:
#ifdef DEBUG
    Serial.println("MAYBE_UP");
#endif
    break;
  case Touch::MAYBE_DOWN:
#ifdef DEBUG
    Serial.println("MAYBE_DOWN");
#endif
    break;
  default:
    Serial.println("PANIC");
  }
  delay(1);



  if (!moved && !selected) {
    // If nothing has been touched for a while, and we are still showing
    // the otp screen, switch back to showing the clock.
    if (!showing_clock_ &&
        (polled_millis - last_touch_millis_ > kIdleTimeout)) {
      showing_clock_ = true;
      d_->clock_poll(t, true, true);
    }
    return;
  }
  last_touch_millis_ = polled_millis;
#ifdef DEBUG
  Serial.println("last touch millis: " + String(last_touch_millis_));
#endif
  if (showing_clock_) {
    showing_clock_ = false;
    d_->refresh(t, true);
    return;
  }

  if (selected) {
    d_->select();
#ifdef DEBUG
    Serial.println("SELECT");
#endif
  } else if (moved && moved_up) {
    d_->up();
  } else if (moved && !moved_up) {
    d_->down();
  }
}
