#include "ota_cb.h"

#include "options.h"

#if USE_WIFI == 1
#include <Adafruit_GFX.h>
#include <ArduinoOTA.h>
#include <TFT_ILI9163C.h>

extern TFT_ILI9163C tft;

void ota_on_start() {
  tft.fillScreen();
  tft.setTextColor(WHITE);  
  tft.setTextSize(1);
  tft.setFont(NULL);
  tft.setCursor(0, 0);
  tft.println("\nOTP loading...");
}

void ota_on_progress(unsigned int  xfered, unsigned int size) {
  static unsigned int last_printed = 0;
  if (xfered > last_printed + 25000) {
    tft.println(String(xfered) + "/" + String(size));
    last_printed += 25000;
  }
}

void ota_on_end() {
  tft.println("OTP finished.");
}

void ota_on_error(ota_error_t e) {
  tft.print("OTP error: ");
  switch (e) {
  case OTA_AUTH_ERROR:
    tft.println("AUTH");
    break;
  case OTA_BEGIN_ERROR:
    tft.println("BEGIN");
    break;
  case OTA_CONNECT_ERROR:
    tft.println("CONNECT");
    break;
  case OTA_RECEIVE_ERROR:
    tft.println("RECEIVE");
    break;
  case OTA_END_ERROR:
    tft.println("END");
    break;
  default:
    tft.println("Unknown: " + String((int)e));
  }
}
#endif
