#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiSPIFFSConfig.h>

void ota_on_start() {
  Serial.println("\n\nOTA starting");
}

void ota_on_progress(unsigned int xfered, unsigned int size) {
  static unsigned int last_printed = 0;
  if (xfered > last_printed + 10000) {
    Serial.print("\r" + String(xfered) + "/" + String(size));
    Serial.flush();
    last_printed += 10000;
  }
}

void ota_on_end() {
  Serial.println("\nOTA finished.");  
}

void setup() {
  Serial.begin(115200);
  WiFiConfig.begin();
  ArduinoOTA.setHostname(WiFiConfig.hostname());
  ArduinoOTA.setPassword(WiFiConfig.ota_password());
  ArduinoOTA.onStart(ota_on_start);
  ArduinoOTA.onProgress(ota_on_progress);
  ArduinoOTA.onEnd(ota_on_end);
  ArduinoOTA.begin();
}

uint32_t last_millis = 0;
void loop() {
  uint32_t m = millis();
  if ((m - last_millis) > 1000UL) {
    Serial.print("6: ");
    Serial.println(m);
    last_millis = m;
  }
  ArduinoOTA.handle();
}
