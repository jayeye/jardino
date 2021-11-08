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

* [`kicad`](kicad) Stuff you may find useful if you use the [KiCad EDA](https://kicad.org)

# Licenses'n'things

* Anything here that is entirely mine feel free to use with some
  appropriate attribution. If you find it useful, tell me, or buy me a
  beer when and if we meet. If you make changes, be nice and say who
  you got the original from. If you manage to make money out of it, it
  would be nice to give me a cut, but I have no way of enforcing
  that. The open-source world will be a lot better if we can keep the
  lawyers out of it by playing fair.

* Not everything here is entirely my own writing. Whenever I have used
  other people's work, I have tried to give proper attribution. If I
  have missed something, please submit
  an [issue](https://github.com/jayeye/jardino/issues) and I will fix
  the problem as soon as I can.
