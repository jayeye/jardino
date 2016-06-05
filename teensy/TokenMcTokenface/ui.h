#ifndef _UI_H_
#define _UI_H_

#include "display.h"
#include "secrets.h"
#include "touch.h"

#include <Encoder.h>
#include <Time.h>

#include <vector>

class Menu;

class UI {
public:
  UI();
  UI(Menu* top);
  void begin(Display* d, Touch* t);
  void refresh();
  void poll(time_t t);

private:
  Display* d_;
  Touch* t_;
  Encoder* enc_;
  uint8_t select_;
  int32_t enc_position_;
  Menu* top_menu_;
  uint32_t last_touch_millis_;
  bool showing_clock_;
  uint32_t last_clock_millis_;
};



class MenuItem {
 public:
  MenuItem(char* text)
    : is_menu_(false),
      text_(text) {
  }

  MenuItem(Menu* submenu)
    : is_menu_(true),
      submenu_(submenu) {
  }
  void render();

 private:
  Menu* container_;
  bool is_menu_;
  char* text_;
  Menu* submenu_;
};


class Menu {
 public:
  void render();
 
 private:
  Menu* up_;
  std::vector<MenuItem*> items_;
};





#endif

