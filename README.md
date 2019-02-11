# Contents

* [`libard`](libard) contains Arduino-style
  libraries. The`library.properties` file may restrict the
  architecture for which the libary is good for.  To install any of
  these libraries, make symlinks from your `~/Arduino/libraries`
  directory to each directory under `libard`. Unit tests, where
  present, usually
  expect [googletest](https://github.com/google/googletest).

  - [`DevNull`](libard/DevNull) is a `/dev/null`-like `Stream` subclass.


* [`arduino`](arduino) Generic Arduino sketches, should work on most variants.



* [`teensy`](teensy) Stuff based on
  the [Teensy 3.2](https://www.pjrc.com/store/teensy32.html). _May
  work on other Teensy 3.* modules._

  - [`TokenMcTokenface`](teensy/TokenMcTokenface) is a clock/TOTP Rube Goldberg device.

* [`esp8266`](esp8266) Stuff based on ESP12 family,
  and only tested on
  the [Huzzah 8266](https://www.adafruit.com/product/2471). _May work
  on other ESP12-based boards._

  - [`ammeter`](esp8266/ammeter) An Amps/Volts/Watts meter with a cute little LCD display.

* [`digispark`](digispark) Stuff based on the
  original [DigiSpark](http://digistump.com/category/1). _May also
  work with the Adafruit Trinket or other ATMicro85 boards._
