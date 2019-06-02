#ifndef Button_h
#define Button_h
#include "Arduino.h"

class Button {
  uint32_t t_of_state_2;
  uint32_t t_of_button_change;    
  uint32_t debounce_time;
  uint32_t long_press_time;
  uint8_t pin;
  uint8_t flag;
  bool button_pressed;
  uint8_t state;
  void read();
  
  public:
  Button(int p);//Initialize, p is the pin on ESP32.
  int update();// Return 0 if not pressed, return 1 for short press, return 2 for long press.
};

#endif