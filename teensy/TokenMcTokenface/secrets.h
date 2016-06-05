#ifndef _SECRETS_H_
#define _SECRETS_H_

#include <Arduino.h>

#include <vector>

class Secrets {
 public:
  Secrets();
  void add(const String& name, const String& description, const String& secret);
  const String& name_at(size_t i);
  const String& description_at(size_t i);
  const String& secret_at(size_t i);
  size_t size();
  void tzoffset(uint32_t t);
  uint32_t tzoffset();
 private:
  std::vector<String> names_;
  std::vector<String> descriptions_;
  std::vector<String> secrets_;
  size_t size_;
  uint32_t tzoffset_;
};


#endif
