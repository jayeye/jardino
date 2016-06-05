#include "secrets.h"

#include <Arduino.h>
#include <vector>

Secrets::Secrets() :
  size_(0) {
}

void Secrets::add(const String& name, const String& description, const String& secret) {
  names_.push_back(name);
  descriptions_.push_back(description);
  secrets_.push_back(secret);
  ++size_;
}

const String& Secrets::name_at(size_t i) {
  if (i < names_.size()) {
    return names_[i];
  } else {
    return String();
  }
}

const String& Secrets::description_at(size_t i) {
  if (i < descriptions_.size()) {
    return descriptions_[i];
  } else {
    return String();
  }
}

const String& Secrets::secret_at(size_t i) {
  if (i < secrets_.size()) {
    return secrets_[i];
  } else {
    return String();
  }
}

size_t Secrets::size() {
  return size_;
}

void Secrets::tzoffset(uint32_t t) {
  tzoffset_ = t;
}

uint32_t Secrets::tzoffset() {
  return tzoffset_;
}
