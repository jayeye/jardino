// Author: John Ioannidis <ji@tla.org>
// License: Beer

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>
#include <Time.h>

#include "clockface.h"
#include "datetime.h"
#include "display.h"
#include "pin_assignments.h"
/*#include "totp.h"*/
#include "touch.h"
#include "ui.h"

extern void poll_serial();

Adafruit_ILI9341* tft;
Display *d;
Touch* ts;
UI* ui;

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
#endif
  clock_begin();
  ts = new Touch(kTouchCs, kTouchIrq);
  ts->begin();
  tft = new Adafruit_ILI9341(kTftCs, kTftDc);
  tft->begin();
  d = new Display(tft);
  d->begin();
  ui = new UI();
  ui->begin(d, ts);
  clock_setup();
}   

void loop() {
  time_t t = now();
  poll_serial();  
  ui->poll(t);
}
