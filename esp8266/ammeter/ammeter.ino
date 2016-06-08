//  .========================================.
//  I LED SCLK MOSI  D/C  RST  CS   GND  Vcc I
//  I  *    *    *    *    *    *    *    *  I
//  `==|====|====|====|====|====|====|====|=='
//     |    |    |    |    |    |    |    |
//     |    |    | .--' .--*----+----+----'
//     |    |    | |    |    .--'    |
//     |    |    `-+----+----+-------+--------------.
//     |    `------+----+----+-------+--------------+---------.
//     `-----------+----+----+-------+---------.    |         |
//                 |    |    |       |         |    |         |
//                 |    |    |    .==|=========|====|=========|======================.
//     *    *    * |  * |  * |  * I  *    *    *    *    *    *    *    *    *    *  I *
//                 |    |    |    I GND Vbat  V+   13   12   14   16   EN    A   RST I
//                 |    |    |    I               MOSI MISO SCLK                     I
//                 |    |    |    I                                                  I
//                 |    |    |    I         Adafruit HUZZAH ESP8266 breakout         I
//                 |    |    |    I                                                  I
//                 |    |    |    I                SS             SDA  SCL           I
//                 |    |    |    I GND  LDO  3V3  15    2    0    4    5   RX   TX  I
//     *    *    * |  * |  * |  * I  *    *    *    *    *    *    *    *    *    *  I *
//                 |    |    |    `==|=========|====|====|=========|====|============'
//                 |    |    |       |         |    |    |         |    |
//                 |    `----+-------+---------*----+----+---------+----+---------.
//                 |         `-------+--------------'    |         |    |         |
//                 `-----------------+-------------------'         |    |         |
//                                   `-----------------------------+----+----.    |
//                                                                 |    |    |    |
//                      .------------------------------------------|----|----*    |
//                      |         .---------------------------.    |    |    |    |
//                      |         |         .------------.    |    |    |    |    |
//                      |         |         |            |    |    |    |    |    |
//                      |         |         |         .==|====|====|====|====|====|==.
//                    GROUND     LOAD     POWER       I  *    *    *    *    *    *  I
//                                        SUPPLY      I Vin+ Vin- SDA  SCL  GND  Vcc I
//                                                    I   Adafruit INA219 breakout   I
//                                                    `=============================='

// From: esp8266/hardware/esp8266/2.2.0/variants/adafruit/pins_arduino.h
// static const uint8_t SDA = 4;
// static const uint8_t SCL = 5;
// From: esp8266/hardware/esp8266/2.2.0/variants/generic/common.h
// static const uint8_t SS    = 15;
// static const uint8_t MOSI  = 13;
// static const uint8_t MISO  = 12;
// static const uint8_t SCK   = 14;

#include "options.h"

#include <Adafruit_INA219.h>
#include <ArduinoOTA.h>
#include <Wire.h>

#if USE_WIFI == 1
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiSPIFFSConfig.h>
#include "ota_cb.h"
#if USE_WEBSERVER == 1
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);
#endif
#endif

#if USE_LCD == 1
#include <SPI.h>

#include "fonts.h"
#include "tft_mono.h"

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

const uint8_t kTftCs = 15;
const uint8_t kTftDc = 2;

TFT_Mono tft = TFT_Mono(kTftCs, kTftDc);
#endif

Adafruit_INA219 ina219;

float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;

#if USE_SERIAL == 1
const char* kStartOfText = "<";     // TODO(ji): make it STX ('\02')
const char* kEndOfText = ">\n";     // TODO(ji): make it ETX ('\03')
const char* kRecordSeparator = ","; // TODO(ji): make it RS ('\036')
void serialprint(const String& s) {
  Serial.print(kStartOfText);
  Serial.print(s);
  Serial.print(kEndOfText);
}
#endif

#if USE_WEBSERVER == 1
void handleRoot() {
  String json("{");
  json += "\n  \"s\": ";
  json += String(shuntvoltage);
  json += ",\n  \"b\": ";
  json += String(busvoltage);
  json += ",\n  \"l\": ";
  json += String(loadvoltage);
  json += ",\n  \"c\": ";
  json += String(current_mA);
  json += "\n}";
  server.send(200, "text/json", json);
}
#endif

void setup() {
  Serial.begin(115200);
#if USE_LCD == 1
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);
  tft.setFont(NULL);
  tft.setTextSize(0);
#endif
#if USE_WIFI == 1
  WiFiConfig.begin();
  ArduinoOTA.setHostname(WiFiConfig.hostname());
  ArduinoOTA.setPassword(WiFiConfig.ota_password());
  ArduinoOTA.begin();
  ArduinoOTA.onStart(ota_on_start);
  ArduinoOTA.onProgress(ota_on_progress);
  ArduinoOTA.onEnd(ota_on_end);
  ArduinoOTA.onError(ota_on_error);
#if USE_WEBSERVER == 1
  server.on("/", handleRoot);
  server.begin();
#endif
#endif
  ina219.begin();
}

int t = 0;

void loop(){
#if USE_WIFI == 1
   ArduinoOTA.handle();
#if USE_WEBSERVER == 1
   server.handleClient();
#endif
#endif
  wdt_disable();
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  // Load voltage:
#if USE_SERIAL == 1
  String ser(millis());
  ser += kRecordSeparator;
  ser += String(loadvoltage, 2);
#endif
#if USE_LCD == 1
  tft.setFont(d_font);
  tft.setCursor(0, 0);
  tft.setTextColor(YELLOW, BLACK);
  tft.yAdvance();
  tft.lineprint(loadvoltage, 2, 2);
  tft.clearToEol();
#endif
  // Current
#if USE_SERIAL == 1
  ser += kRecordSeparator;
  ser += String(current_mA, 1);
#endif
#if USE_LCD == 1
  tft.setTextColor(CYAN, BLACK);
  tft.setFont(d_font);
  tft.yAdvance();
  tft.lineprint(current_mA, 4, 1);
  tft.clearToEol();
#endif
  // Power
  float power_W = current_mA * loadvoltage / 1000.0;
#if USE_SERIAL == 1
  ser += kRecordSeparator;
  ser += String(power_W, 3);
  serialprint(ser);
#endif
#if USE_LCD == 1
  tft.setTextColor(GREEN, BLACK);
  tft.setFont(d_font);
  tft.yAdvance();
  tft.lineprint(power_W, 2, 3);
  tft.clearToEol();
#endif
}
