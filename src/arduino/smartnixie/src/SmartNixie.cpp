#include <Arduino.h>
#include <Wire.h>

#include "SmartNixie.h"

const uint8_t SmartNixieClass::kNixieValueMask = 0x0f;
const uint8_t SmartNixieClass::kNixieDPMask = 0x10;

SmartNixieClass::SmartNixieClass() {
}

void SmartNixieClass::begin(uint8_t num_nixies, uint8_t base_address) {
  num_nixies_ = num_nixies;
  base_address_ = base_address;
  values_ = new uint8_t [num_nixies_];
  for (int i = 0; i < num_nixies_; ++i) {
    values_[i] = 0;
  }
  cylon_dot_ = 0;
  cylon_increasing_ = true;
  cylon_millis_ = 0;
}

void SmartNixieClass::begin(uint8_t num_nixies, uint8_t base_address,
                            int sda, int scl) {
  Wire.begin(sda, scl);
  begin(num_nixies, base_address);
}

void SmartNixieClass::show(uint8_t nixie) {
  uint8_t hi = 0;
  uint8_t lo = 0;
  uint8_t v = values_[nixie];
  if (v & kNixieDPMask) {
    hi = 0x04;
  }
  v &= kNixieValueMask;
  if (v <= 7) {
    lo = 1 << v;
  } else if (v <= 9) {    
    hi |= (1 << (v - 8));
  } else if (v == 10) {
    hi = 0x04;
  } else {
    lo = hi = 0;
  }
  Wire.beginTransmission(nixie + base_address_);
  Wire.write(1);  // register address
  Wire.write(hi);
  Wire.write(lo);
  Wire.endTransmission(true);
}

void SmartNixieClass::put(uint8_t nixie, uint8_t value) {
  if (nixie >= num_nixies_) {
    return;
  }
  values_[nixie] = (values_[nixie] & ~kNixieValueMask) | 
    (value & kNixieValueMask);
  show(nixie);
}

void SmartNixieClass::dp(uint8_t nixie, bool dp) {
  if (nixie >= num_nixies_) {
    return;
  }
  if (dp) {
    values_[nixie] |= kNixieDPMask;
  } else {
    values_[nixie] &= ~kNixieDPMask;
  }
  show(nixie);
}

void SmartNixieClass::blank() {
  for (int i = 0; i < num_nixies_; ++i) {
    put(i, 11);
  }
}

void SmartNixieClass::cylon(uint32_t every_ms) {
  // Rate-limit
  if (millis() - cylon_millis_ < every_ms) {
    return;
  }
  cylon_millis_ = millis();
  dp(cylon_dot_, false);
  if (cylon_increasing_) {
    if (cylon_dot_ < num_nixies_ - 1) {
      ++cylon_dot_;
    } else {
      cylon_dot_ = num_nixies_ - 2;
      cylon_increasing_ = false;
    }
  } else {
    if (cylon_dot_ > 0) {
      --cylon_dot_;
    } else {
      cylon_dot_ = 1;
      cylon_increasing_ = true;
    }
  }
  dp(cylon_dot_, true);
}

SmartNixieClass SmartNixie;
