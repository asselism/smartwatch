#include "Arduino.h"
#include "Button.h"

Button::Button(int p) {
  flag = 0;  
  state = 0;
  pin = p;
  t_of_state_2 = millis();
  t_of_button_change = millis();
  debounce_time = 10;
  long_press_time = 1000;
  button_pressed = 0;
}

void Button::read() {
  uint8_t button_state = digitalRead(pin);  
  button_pressed = !button_state;
}

int Button::update() {
//the code was omitted because this project was completed as part of
//Embedded Systems (6.08) class. The current function was written
//as part of a homework exercise and deleted as per class's sharing policy
}

