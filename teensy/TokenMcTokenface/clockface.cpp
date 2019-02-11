#include "clockface.h"
#include "display.h"
#include "droid.h"
#include "miscmono.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>
//#include <Adafruit_ILI9341.h>
#include <ILI9341_t3.h>
#include <Time.h>


extern Adafruit_ILI9341* tft;
extern Display* d;
static String previous;
static int day_previous;
static const uint16_t date_x = 1;
static const uint16_t date_y = 230;

static uint16_t ccenterx;
static uint16_t ccentery;
static uint16_t cradius;
static const float scosConst = 0.0174532925;
static float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;
static float sdeg=0, mdeg=0, hdeg=0;
static uint16_t osx, osy, omx, omy, ohx, ohy;
static uint16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;
static uint16_t kGold;




void drawClockFace(){
  tft->fillCircle(ccenterx, ccentery, cradius, kGold);
  tft->fillCircle(ccenterx, ccentery, cradius - 4, ILI9341_BLACK);
  for (int i = 0; i < 360; i += 30) {
    sx = cos((i - 90) * scosConst);
    sy = sin((i - 90) * scosConst);
    x0 = sx * (cradius - 4) + ccenterx;
    yy0 = sy * (cradius - 4) + ccentery;
    x1 = sx * (cradius - 16) + ccenterx;
    yy1 = sy * (cradius - 16) + ccentery;
    tft->drawLine(x0, yy0, x1, yy1, kGold);
  }
}

void drawClockHands(time_t t, bool draw_seconds) {
  uint8_t h = hour(t);
  uint8_t m = minute(t);
  uint8_t s = second(t);

  sdeg = s * 6;                  // 0-59 -> 0-354
  mdeg = m * 6 + sdeg * 0.01666667;  // 0-59 -> 0-360 - includes seconds
  hdeg = h * 30 + mdeg * 0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
  hx = cos((hdeg - 90) * scosConst);
  hy = sin((hdeg - 90) * scosConst);
  mx = cos((mdeg - 90) * scosConst);
  my = sin((mdeg - 90) * scosConst);
  sx = cos((sdeg - 90) * scosConst);
  sy = sin((sdeg - 90) * scosConst);
  // Erase just old hand positions
  tft->drawLine(ohx, ohy, ccenterx+1, ccentery+1, ILI9341_BLACK);
  tft->drawLine(omx, omy, ccenterx+1, ccentery+1, ILI9341_BLACK);
  if (draw_seconds) {
    tft->drawLine(osx, osy, ccenterx+1, ccentery+1, ILI9341_BLACK);
  }
  // Draw new hand positions
  tft->drawLine(hx * (cradius - 28) + ccenterx + 1,
                hy * (cradius - 28) + ccentery + 1,
                ccenterx + 1,
                ccentery + 1,
                ILI9341_WHITE);
  tft->drawLine(mx * (cradius - 17) + ccenterx + 1,
                my * (cradius - 17) + ccentery + 1,
                ccenterx + 1,
                ccentery + 1,
                ILI9341_WHITE);
  if (draw_seconds) {
    tft->drawLine(sx * (cradius - 14) + ccenterx + 1,
                  sy * (cradius - 14) + ccentery + 1,
                  ccenterx + 1,
                  ccentery + 1,
                  ILI9341_RED);
  }
  tft->fillCircle(ccenterx+1, ccentery+1, 3, ILI9341_RED);
  // Update old x&y coords
  osx = sx * (cradius - 14) + ccenterx + 1;
  osy = sy * (cradius - 14) + ccentery + 1;
  omx = mx * (cradius - 17) + ccenterx + 1;
  omy = my * (cradius - 17) + ccentery + 1;
  ohx = hx * (cradius - 28) + ccenterx + 1;
  ohy = hy * (cradius - 28) + ccentery + 1;
}

void clock_setup() {
  kGold = tft->color565(212, 175, 55);
  ccenterx = tft->width() / 2;
  ccentery = (date_y - 10) / 2;
  cradius = ccentery - 10;
  osx = ccenterx;
  osy = ccentery;
  omx = ccenterx;
  omy = ccentery;
  ohx = ccenterx;
  ohy = ccentery;
  tft->fillScreen(ILI9341_BLACK);
  drawClockFace();
  previous = " ";
  day_previous = 0;
}

void clock_poll(time_t t, bool draw_seconds) {
  drawClockHands(t, draw_seconds);
  if (day(t) == day_previous) {
    return;
  }
  // Largest font that fits; test with "Wednesday, September 31st, 2016"
  tft->setFont(DroidSans_12);
  d->jshow(previous.c_str(), d->width() / 2, d->height() - 1,
           Display::CENTER_JUSTIFY, Display::BOTTOM_JUSTIFY,
           ILI9341_BLACK, ILI9341_BLACK);
  String current = dayStr(weekday(t)) + String(",  ") +
                   monthStr(month(t)) + String("  ") +
                   String(day(t));
  switch (day(t) % 10) {
  case 1:
    current += "st";
    break;
  case 2:
    current += "nd";
    break;
  case 3:
    current += "rd";
    break;
  default:
    current += "th";
  }
  current += ",  ";
  current += String(year(t));
  d->jshow(current.c_str(),
           d->width() / 2, d->height() - 1,
           Display::CENTER_JUSTIFY, Display::BOTTOM_JUSTIFY,
           ILI9341_CYAN, ILI9341_BLACK);
  previous = current;
  day_previous = day(t);
}
