#ifndef _SMARTNIXIE_H_
#define _SMARTNIXIE_H_

#include <Arduino.h>
#include <Wire.h>

class SmartNixieClass {
 public:
  SmartNixieClass();
  void begin(uint8_t num_nixies, uint8_t base_address);
  void begin(uint8_t num_nixies, uint8_t base_address, int sda, int scl);
  void dp(uint8_t nixie, bool dp);
  void put(uint8_t nixie, uint8_t value);
  void blank();
  void cylon(uint32_t every_ms=100);
  uint8_t read_address(uint8_t nixie);

private:
  void show(uint8_t nixie);
  uint8_t num_nixies_;
  uint8_t base_address_;
  uint8_t cylon_dot_;
  bool cylon_increasing_;
  uint32_t cylon_millis_;
  uint8_t *values_;
  static const uint8_t kNixieValueMask;
  static const uint8_t kNixieDPMask;
  // Disallow evil constructors!
  SmartNixieClass(const SmartNixieClass&);
  void operator=(const SmartNixieClass&);
};

extern SmartNixieClass SmartNixie;
#endif
