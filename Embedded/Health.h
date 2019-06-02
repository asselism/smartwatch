#ifndef Health_H
#define Health_H

#include "Arduino.h"
#include <TFT_eSPI.h>

class Health_Module {
  TFT_eSPI* tft;
  bool is_active;
  int state;
  int measurement_Pin;
  int raw_data=0;
  float averaged_data=0;
  float baseline=0;
  float variance=0;
  float std=0;
  float deviation=0;
  int start_time;
  int end_time;
  int number_cross_threshold;
  int last_state_change_timer;
  int max=0;
  int min=0;
  bool is_increasing;
  
  public:
  Health_Module(TFT_eSPI* tft_to_use, int measurementPin);
  void activate();
  void update(int bv);
  void close();
};

#endif