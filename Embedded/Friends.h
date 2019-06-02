#ifndef Friends_h
#define Friends_h

#include "Arduino.h"
#include <TinyGPS++.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "Type_with_character.h"
#include "Requests.h"

class Message_Module{
  TFT_eSPI* tft;
  MPU9255* imu;
  int number_of_messages;
  int start_of_messages;//start position(cyclic)
  char* messages_buffer[10];
  bool is_active;
  int state;
  Type_C_Module type_module;
  char message_to_type[200];
  char response[1000];
  char request[1000];
  char body[200];
  uint32_t check_message_timer;
  char* user;

  public:
  bool do_not_request=false;
  Message_Module(TFT_eSPI* tft_to_use, MPU9255* imu_to_use,char* username);
  void activate();
  void close();
  void update(int bv);
};

class Nearby_Module {
  TFT_eSPI* tft;
  TinyGPSPlus* gps;
  bool is_active;
  char response[1000];
  char request[1000];
  char body[200];
  uint32_t timer;
  char* user;

  public:
  bool do_not_request=false;
  Nearby_Module(TFT_eSPI* tft_to_use, TinyGPSPlus* gps_to_use,char* username);
  void activate();
  void close();
  void update(int bv);
};

class Add_Module {
  TFT_eSPI* tft;
  MPU9255* imu;
  int number_of_requests;
  int start_of_requests;//start position(cyclic)
  char* requests_buffer[10];
  bool is_active;
  int state;
  Type_C_Module type_module;
  char message_to_type[200];
  char response[1000];
  char request[1000];
  char body[200];
  uint32_t check_requests_timer;
  char* user;

  public:
  bool do_not_request=false;
  Add_Module(TFT_eSPI* tft_to_use, MPU9255* imu_to_use,char* username);
  void activate();
  void close();
  void update(int bv);
};

class Friends_Module {
  bool last_wifi_state=true;
  TFT_eSPI* tft;
  int state;//0 is not active, 1 is message, 2 is microphone(deleted), 3 is nearby, 4 is add

  public:
  Message_Module message_module;
  Nearby_Module nearby_module;
  Add_Module add_module;
  Friends_Module(TFT_eSPI* tft_to_use, TinyGPSPlus* gps_to_use, MPU9255* imu_to_use, char* username);
  void activate(int a);
  void close();
  void update(int bv, bool is_wifi_connected);
};

#endif