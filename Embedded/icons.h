#ifndef icons_h
#define icons_h
#include "Arduino.h"
#include <TinyGPS++.h>
#include <TFT_eSPI.h>
#include <math.h>

class Battery_icon {
  TFT_eSPI* screen;
  uint8_t x;
  uint8_t y;
  uint16_t fc;
  uint16_t bgc;
  uint16_t tc;
  
  int last_update;
  double voltage_from_discharge(double discharge);
  double discharge_from_voltage(double voltage, double error);
  void drawBattery(float level);
  
  public:
  bool need_to_draw_immediately=true;
  bool is_in_home_page=true;
  Battery_icon(TFT_eSPI* tft, uint8_t x_loc, uint8_t y_loc, uint16_t color, uint16_t background, uint16_t text_c);
  void update(); // draw the battery
};


class WiFi_icon {
  TFT_eSPI* screen;
  uint8_t x;
  uint8_t y;
  uint16_t c_on;
  uint16_t c_off;
  uint16_t bc;  

  public:
  uint8_t status;
  WiFi_icon(TFT_eSPI* tft, uint8_t x_loc, uint8_t y_loc, uint16_t color_on, uint16_t color_off, uint16_t bg_c);
  void drawWiFi();
};

class GPS_icon {
  TinyGPSPlus* gps;
  TFT_eSPI* screen;
  uint8_t x;
  uint8_t y;
  uint16_t c_on;
  uint16_t c_off;
  uint16_t tc;

  public:
  uint8_t status;
  uint8_t old_status;

  GPS_icon(TinyGPSPlus* GPS_to_use, TFT_eSPI* tft, uint8_t x_loc, uint8_t y_loc, uint16_t color_on, uint16_t color_off, uint16_t text_c);
  void get_GPS_status();
  void drawGPS(); //actually draw the icon
  void update(); //use in loop(), only update when changes
};

#endif
