#include "Type.h"

Type_Module::Type_Module(char* message_to_type, MPU9255* imu_to_use) {
  char_index=0;
  state=0;
  message=message_to_type;
  message[0]=char(0);
  imu=imu_to_use;
  scrolling_timer=millis();
}

int Type_Module::update(int bv) {
//the code was omitted because this project was completed as part of
//Embedded Systems (6.08) class. The current function was written
//as part of a homework exercise and deleted as per class's sharing policy
}

void Type_Module::reset() {
  char_index=0;
  state=0;
  message[0]=char(0);
  scrolling_timer=millis();
}
