#ifndef _DEVNULL_H_
#define _DEVNULL_H_
#include <Arduino.h>
#include <Stream.h>
class DevClass : public Stream {
public:
  virtual inline int read() { return 0; }
  virtual inline int peek() { return 0; }
  virtual inline void flush() { return; }
};

class DevNullClass : public DevClass {
public:
  virtual inline int available() { return 0; }
  virtual inline size_t write(uint8_t c) { return 0; }
};

class DevZeroClass : public DevClass {
public:
  virtual inline int available() { return 1; }
  virtual inline size_t write(uint8_t c) { return 1; }
};

extern DevNullClass DevNull;
extern DevZeroClass DevZero;
#endif
