#ifndef Type_C_H
#define Type_C_H

#include "Arduino.h"
#include <mpu9255_esp32.h>
#include<math.h>
#include<string.h>

class Type_C_Module {
  char alphabet[50] = " abcdefghijklmnopqrstuvwxyz0123456789";
  int char_index;
  unsigned long scrolling_timer;
  const int scrolling_threshold = 150;
  const float angle_threshold = 0.3;
  int state;
  char* message;
  MPU9255* imu;
  
  public:
  Type_C_Module(char* message_to_type, MPU9255* imu_to_use);//It will record what you typed into message_to_type.
  int update(int bv);//return 0 if *message is not changed; return 1 if *message is changed, but typing not finished; return 2 if typing finished.
  void reset();
};

#endif