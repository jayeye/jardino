#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <FS.h>
extern "C" {
#include <user_interface.h>
}
#include "WiFiSPIFFSConfig.h"

void error_blink(uint8_t slowblinks, uint8_t fastblinks, bool loop) {
  pinMode(0, OUTPUT);
  do {
    for (int i = 0; i < slowblinks; ++i) {
      digitalWrite(0, LOW);
      delay(500);
      digitalWrite(0, HIGH);
      delay(500);
    }
    for (int i = 0; i < fastblinks; ++i) {
      digitalWrite(0, LOW);
      delay(150);
      digitalWrite(0, HIGH);
      delay(150);
    }
    delay(1000);
  } while(loop);
}

void error_blink(uint16_t blinks, bool loop) {
  error_blink(blinks >> 8, blinks & 0xff, loop);
}

static const char* reason(int r) {
  switch (r) {
  case REASON_UNSPECIFIED: return "REASON_UNSPECIFIED";
  case REASON_AUTH_EXPIRE: return "REASON_AUTH_EXPIRE";
  case REASON_AUTH_LEAVE: return "REASON_AUTH_LEAVE";
  case REASON_ASSOC_EXPIRE: return "REASON_ASSOC_EXPIRE";
  case REASON_ASSOC_TOOMANY: return "REASON_ASSOC_TOOMANY";
  case REASON_NOT_AUTHED: return "REASON_NOT_AUTHED";
  case REASON_NOT_ASSOCED: return "REASON_NOT_ASSOCED";
  case REASON_ASSOC_LEAVE: return "REASON_ASSOC_LEAVE";
  case REASON_ASSOC_NOT_AUTHED: return "REASON_ASSOC_NOT_AUTHED";
  case REASON_DISASSOC_PWRCAP_BAD: return "REASON_DISASSOC_PWRCAP_BAD";
  case REASON_DISASSOC_SUPCHAN_BAD: return "REASON_DISASSOC_SUPCHAN_BAD";
  case REASON_IE_INVALID: return "REASON_IE_INVALID";
  case REASON_MIC_FAILURE: return "REASON_MIC_FAILURE";
  case REASON_4WAY_HANDSHAKE_TIMEOUT: return "REASON_4WAY_HANDSHAKE_TIMEOUT";
  case REASON_GROUP_KEY_UPDATE_TIMEOUT: return "REASON_GROUP_KEY_UPDATE_TIMEOUT";
  case REASON_IE_IN_4WAY_DIFFERS: return "REASON_IE_IN_4WAY_DIFFERS";
  case REASON_GROUP_CIPHER_INVALID: return "REASON_GROUP_CIPHER_INVALID";
  case REASON_PAIRWISE_CIPHER_INVALID: return "REASON_PAIRWISE_CIPHER_INVALID";
  case REASON_AKMP_INVALID: return "REASON_AKMP_INVALID";
  case REASON_UNSUPP_RSN_IE_VERSION: return "REASON_UNSUPP_RSN_IE_VERSION";
  case REASON_INVALID_RSN_IE_CAP: return "REASON_INVALID_RSN_IE_CAP";
  case REASON_802_1X_AUTH_FAILED: return "REASON_802_1X_AUTH_FAILED";
  case REASON_CIPHER_SUITE_REJECTED: return "REASON_CIPHER_SUITE_REJECTED";
  case REASON_BEACON_TIMEOUT: return "REASON_BEACON_TIMEOUT";
  case REASON_NO_AP_FOUND: return "REASON_NO_AP_FOUND";
  case REASON_AUTH_FAIL: return "REASON_AUTH_FAIL";
  case REASON_ASSOC_FAIL: return "REASON_ASSOC_FAIL";
  case REASON_HANDSHAKE_TIMEOUT: return "REASON_HANDSHAKE_TIMEOUT";
  }
  return "REASON NOT FOUND!!!!!";
}

static const char* wl_status(wl_status_t s) {
  switch (s) {
  case WL_NO_SHIELD: return "WL_NO_SHIELD";
  case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
  case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
  case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
  case WL_CONNECTED: return "WL_CONNECTED";
  case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
  case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
  case WL_DISCONNECTED: return "WL_DISCONNECTED";
  }
  return "WL_STATUS NOT FOUND!!!!!";
}



static Stream*  event_handler_logger;

static void event_handler(System_Event_t *event) {
  event_handler_logger->print("\r\n******** at " + String(millis()) + "ms: ");
  switch (event->event) {
  case EVENT_STAMODE_CONNECTED:
    event_handler_logger->print("ESP8266 station connected to AP channel ");
    event_handler_logger->print(event->event_info.connected.channel, DEC);
    break;
  case EVENT_STAMODE_DISCONNECTED:
    event_handler_logger->print("ESP8266 station disconnected from AP: ");
    event_handler_logger->print(reason(event->event_info.disconnected.reason));
    WiFi.disconnect();
    event_handler_logger->print("... reconnecting.");
    WiFi.reconnect();
    break;
  case EVENT_STAMODE_AUTHMODE_CHANGE:
    event_handler_logger->print("the auth mode of AP connected by ESP8266 station changed");
    break;
  case EVENT_STAMODE_GOT_IP:
    event_handler_logger->print("ESP8266 station got IP from connected AP: inet ");
    event_handler_logger->print(IPAddress(event->event_info.got_ip.ip.addr));
    event_handler_logger->print(" mask ");
    event_handler_logger->print(IPAddress(event->event_info.got_ip.mask.addr));
    event_handler_logger->print(" gw ");
    event_handler_logger->print(IPAddress(event->event_info.got_ip.gw.addr));
    break;
  case EVENT_STAMODE_DHCP_TIMEOUT:
    event_handler_logger->print("ESP8266 station dhcp client got IP timeout");
    break;
  case EVENT_SOFTAPMODE_STACONNECTED:
    event_handler_logger->print("a station connected to ESP8266 soft-AP");
    break;
  case EVENT_SOFTAPMODE_STADISCONNECTED:
    event_handler_logger->print("a station disconnected to ESP8266 soft-AP");
    break;
  case EVENT_SOFTAPMODE_PROBEREQRECVED:
    event_handler_logger->print("Receive probe request packet in soft-AP interface");
    break;
  default:
    event_handler_logger->print("Unknown event type: " + String((int)event->event));
  }
  event_handler_logger->println();
}

WiFiSPIFFSConfig_class::WiFiSPIFFSConfig_class()
  : hostname_(nullptr),
    ota_password_(nullptr),
    password_(nullptr),
    ssid_(nullptr),
    blink_on_error_(true),
    announce_mdns_(true) {
  logger(&Serial);
  wifi_set_event_handler_cb(event_handler);
}

int WiFiSPIFFSConfig_class::error(uint16_t error_code) {
  error_blink(error_code, blink_on_error_);  // May not return, of course!
  return error_code;
}

int WiFiSPIFFSConfig_class::load_config() {
  if (!SPIFFS.begin()) {
    return error(BAD_SPIFFS);
  }
  File config_file = SPIFFS.open("/config.json", "r");
  if (!config_file) {
    return error(NO_CONFIG_JSON);
  }
  size_t size = config_file.size();
  if (size > 1024) {
    return error(CONFIG_TOO_BIG);
  }
  std::unique_ptr<char[]> buf(new char[size]);
  config_file.readBytes(buf.get(), size);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());
  if (!json.success()) {
    return error(CANT_READ_CONFIG_JSON);
  }
  const char* tmp;
  if (hostname_ == nullptr) {
    tmp = (const char*)json["hostname"];
    if (tmp == nullptr) {
      return error(NO_HOSTNAME);
    } else {
      hostname_ = strdup(tmp);
    }
  }
  if (ssid_ == nullptr) {
    tmp = (const char*)json["ssid"];
    if (tmp == nullptr) {
      return error(NO_SSID);
    } else {
      ssid_ = strdup(tmp);
    }
  }
  if (password_ == nullptr) {
    tmp = (const char*)json["password"];
    if (tmp == nullptr) {
      return error(NO_PASSWORD);
    } else {
      password_ = strdup(tmp);
    }
  }
  if (ota_password_ == nullptr) {
    tmp = (const char*)json["ota_password"];
    if (tmp != nullptr) {
      ota_password_ = strdup(tmp);
    }
  }
  logger_->print("Configuration:\r\n  hostname: " +
                 String(hostname_) +
                 "\r\n  ssid: " +
                 String(ssid_) +
                 "\r\n  password: " +
                 String(password_));
  if (ota_password_ != nullptr) {
    logger_->print("\r\n  ota_password: " + String(ota_password_));
  }
  logger_->println();
  return 0;
}

void WiFiSPIFFSConfig_class::mdns_off() {
  announce_mdns_ = false;
}

void WiFiSPIFFSConfig_class::hostname(const char* hostname) {
  hostname_ = hostname;
}

const char* WiFiSPIFFSConfig_class::hostname() {
  return hostname_;
}

void WiFiSPIFFSConfig_class::logger(Stream* logger) {
  logger_ = logger;
  event_handler_logger = logger_;
}

void WiFiSPIFFSConfig_class::password(const char* password) {
  password_ = password;
}

const char* WiFiSPIFFSConfig_class::password() {
  return password_;
}

void WiFiSPIFFSConfig_class::ota_password(const char* ota_password) {
  ota_password_ = ota_password;
}

const char* WiFiSPIFFSConfig_class::ota_password() {
  return ota_password_;
}

void WiFiSPIFFSConfig_class::ssid(const char* ssid) {
  ssid_ = ssid;
}

const char* WiFiSPIFFSConfig_class::ssid() {
  return ssid_;
}

bool WiFiSPIFFSConfig_class::begin(bool blink_on_error) {
  blink_on_error_ = blink_on_error;
  if (load_config() != 0) {
    return false;
  }
  WiFi.hostname(hostname_);
  do {
    // Connect to WiFi network
    logger_->print("WiFi.begin() returns ");
    logger_->println(wl_status(WiFi.begin(ssid_, password_)));
    // Wait for connection
    error_blink(0, 1, false);
    delay(100);
  } while (WiFi.status() != WL_CONNECTED);

  if (announce_mdns_) {
    if (!MDNS.begin(hostname_)) {
      return error(MDNS_FAIL);
    }
    logger_->println("mDNS responder started");
  }
  return 0;
}  

WiFiSPIFFSConfig_class   WiFiConfig;
