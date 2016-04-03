#include <SmartNixie.h>

void setup() {
  SmartNixie.begin(6, 8, 5, 4);
  SmartNixie.blank();
  for (int i = 0; i < 6; ++i) {
    SmartNixie.put(i, i);
  }
  delay(2000);
}

void loop() {
  SmartNixie.cylon(500);
}
