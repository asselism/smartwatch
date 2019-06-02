    #ifndef Steps_h
    #define Steps_h
    #include "Arduino.h"
    #include <TFT_eSPI.h>
    #include <mpu9255_esp32.h>
    #include <string.h>
    #include "Button.h"
    #include "Type.h"
    #include "Time.h"
    class Steps_Module
    {
      TFT_eSPI* tft;
      MPU9255* imu;
      char* user;
      Time* time_module;
      
      bool is_active=false;
      bool is_initialized=false;


      int steps=0;
      int last_steps=0;
      int history_data[10];
      int last_post_data=0;
      int year;
      int month;
      int day;

      char message_to_type[10]={0};
      Type_Module type_module;


      char body[200];
      char request[1000];
      char response[1000];

      
      int state=0;
      int counter_state=0;
      uint32_t post_timer;


      void initialize();
      void draw();
      
      public:
      bool do_not_request=false;
      Steps_Module(TFT_eSPI* tft_to_use, MPU9255* imu_to_use, char* username, Time* time_module_to_use);
      void activate();
      void close();
      void update(int bv, bool is_wifi_connected);
      int get_steps();
    };
    #endif
