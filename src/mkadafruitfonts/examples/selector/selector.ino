#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Encoder.h>
#include "droid.h"
#include "miscmono.h"

const uint8_t kTftCs = 2;
const uint8_t kTftDc = 4;
const uint8_t kTouchCs = 10;
const uint8_t kTouchIrq = 9;
const uint8_t kSdCs = 20;
const uint8_t kRotaryLeft = 22;
const uint8_t kRotaryRight = 23;
const uint8_t kRotarySelect = 3;

const GFXfont* myfonts[] = {Inconsolata_Bold_48,
                            DroidSans_Bold_42,
                            Inconsolata_Regular_48,
                            DroidSansMono_42,
                            KernelPanic_42,
                            Monoid_42,
                            Pennstat_42,
                            Repetition_42,
};
const char* fontnames[] = {"Inconsolata_Bold_48",
                           "DroidSans_Bold_42",
                           "Inconsolata_Regular_48",
                           "DroidSansMono_42",
                           "KernelPanic_42",
                           "Monoid_42",
                           "Pennstat_42",
                           "Repetition_42",
};
int fontNFONT = sizeof myfonts / sizeof myfonts[0];
                           
Adafruit_ILI9341 tft(kTftCs, kTftDc);
Encoder enc(kRotaryLeft, kRotaryRight);

void jshow(const char*s,
           int x, int y,
           int h_justify, int v_justify,
           uint16_t text_color, uint16_t bg_color) {
  int16_t gt_x, gt_y;
  uint16_t gt_w, gt_h;
  tft.getTextBounds(s, 0, 0, &gt_x, &gt_y, &gt_w, &gt_h);
  int hj = gt_w * (h_justify - 1) / 2;
  int vj = gt_h * (v_justify - 1) / 2;
  tft.fillRect(x + hj, y + vj, gt_w, gt_h, bg_color);
  tft.setTextColor(text_color, bg_color);
  tft.setCursor(x - gt_x + hj, y - gt_y + vj);
  tft.print(s);
}

int32_t pos = 0;
int cursor = 0;

void setup() {
  pinMode(kSdCs, OUTPUT);
  digitalWrite(kSdCs, HIGH);
  pinMode(kTouchCs, OUTPUT);
  digitalWrite(kTouchCs, HIGH);
  while (!Serial);
  Serial.begin(115200);
  Serial.println("starting");
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
}

void loop() {
  int32_t newpos = enc.read();
  if (newpos != pos) {
    delay(100);
    if ((newpos < pos) && (cursor > 0)) {
      --cursor;
    }
    if ((newpos > pos) && (cursor < fontNFONT - 1)) {
      ++cursor;
    }
    pos = newpos;

    tft.fillScreen(ILI9341_BLACK);
    tft.setFont(DroidSans_9);
    jshow(fontnames[cursor], 0, 0, 1, 1, ILI9341_YELLOW, ILI9341_BLUE);
    tft.setFont(myfonts[cursor]);
    jshow("HEN3RY", 160, 120, 0, 0, ILI9341_YELLOW, ILI9341_BLUE);
  }
}
