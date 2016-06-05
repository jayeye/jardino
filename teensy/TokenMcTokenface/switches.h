// Author: John Ioannidis <ji@tla.org>
// License: Beer

#ifndef _SWITCHES_H_
#define _SWITCHES_H_
class Switches {
 public:
  Switches();
  void begin();
  void poll();
  int swN(int n);
  int rot();
};

#endif
