#ifndef _WIFISPIFFSCONFIG_H_
#define _WIFISPIFFSCONFIG_H_

#include <DevNull.h>
#include <ESP8266WiFi.h>
#include <Stream.h>

extern void error_blink(uint8_t slowblinks, uint8_t fastblinks, bool loop=true);
extern void error_blink(uint16_t blinks, bool loop=true);

class WiFiSPIFFSConfig_class {
public:
  enum config_errors {
    BAD_SPIFFS = 0x0101,
    NO_CONFIG_JSON = 0x0102,
    CONFIG_TOO_BIG = 0x0103,
    CANT_READ_CONFIG_JSON = 0x0104,
    NO_HOSTNAME = 0x0201,
    NO_SSID = 0x0202,
    NO_PASSWORD = 0x0203,
    MDNS_FAIL = 0x0301,
  };
  WiFiSPIFFSConfig_class();
  bool begin(bool blink_on_error=false);
  const char* hostname();
  int error(uint16_t error_code);
  void hostname(const char* hostname);
  void logger(Stream* logger);
  void mdns_off();
  const char* ota_password();
  void ota_password(const char* ota_password);
  const char* password();
  void password(const char* password);
  const char* ssid();
  void ssid(const char* ssid);
private:
  int load_config();
  const char* hostname_;
  const char* ota_password_;
  const char* password_;
  const char* ssid_;
  Stream* logger_;
  bool blink_on_error_;
  bool announce_mdns_;
};

extern WiFiSPIFFSConfig_class WiFiConfig;
#endif
