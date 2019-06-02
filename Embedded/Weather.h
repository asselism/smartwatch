#ifndef Weather_H
#define Weather_H
#include "Arduino.h"
#include "Requests.h"
#include <TinyGPS++.h>

class Weather {
  public:
    int lat_deg; //degrees portion of lattitude
    float lat_dm; //latitude decimal minutes
    char lat_dir; //latitude direction
    int lon_deg; //longitude in degrees
    float lon_dm; //longitude decimal minutes
    char lon_dir; //longitude direction
    float sign_lat_deg; //signed value of latitude (minutes are decimals)
    float sign_lon_deg; //signed value of longtitude (minutes are decimals)
    int year; //year
    int month; //month
    int day; //day of month
    int hour; //hour (24 clock GMT)
    int minute; //minute
    int second; //second
    bool valid; //is the GPS data valid
    TFT_eSPI* tft;
    TinyGPSPlus* gps; //instantiate approporiate Serial object for GPS 
    uint32_t weather_timer;
    const uint16_t WAIT_PERIOD = 4000; //how much to wait for GPS signal

    //Some constants and some resources:
    const int WEATHER_RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
    const uint16_t WEATHER_IN_BUFFER_SIZE = 3000; //size of buffer to hold HTTP request
    const uint16_t WEATHER_OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP get response
    const uint16_t PRINT_LEN = 20; //size of print buffer to hold the char arrays with info to display on the screen
    char weather_request_buffer[3000]; //char array buffer to hold HTTP request
    char weather_response_buffer[1000]; //char array buffer to hold HTTP post response
    char print_buffer[20][100] = {{'\0'}}; //array of char arrays with info to display on the screen
    const int WEATHER_BUFFER_LENGTH = 200;  //size of char array we'll use for
    char weather_buffer[200] = {0}; //dumping chars into

    int ind = 0; //the index where the new entry should go the print_weather_buffer

    Weather(TFT_eSPI* tft_to_use, TinyGPSPlus* gps_to_use) {
      tft = tft_to_use;
      gps = gps_to_use;
      start();
    }

    void get_request(char * key) { //request to 608dev
      sprintf(weather_request_buffer, "GET http://608dev.net/sandbox/sc/asselism/weather.py?lat=%2.4f&lon=%3.4f&key=%s HTTP/1.1\r\n", sign_lat_deg, sign_lon_deg, key);
      strcat(weather_request_buffer, "Host: 608dev.net\r\n"); //add more to the end
      strcat(weather_request_buffer, "\r\n"); //add blank line!
      Serial.println(weather_request_buffer);
      do_http_request("608dev.net", weather_request_buffer, weather_response_buffer, WEATHER_OUT_BUFFER_SIZE, WEATHER_RESPONSE_TIMEOUT, true);
      Serial.println(weather_response_buffer);
    }

     void start() {
      if (gps->location.isValid()) {
        sign_lon_deg =  gps->location.lng();
        sign_lat_deg =  gps->location.lat();
      }
      else {
        sign_lon_deg =  -71.1097335;
        sign_lat_deg =  42.3736158;
      }
    }

    void get_vis() { //for getting the visibility info
      char key[] = "vis";
      get_request(key);
      if(weather_response_buffer[0]=='\0') {
        sprintf(weather_response_buffer,"Sunny");
      }
    }

    float get_temp() { //for getting the temperature info
      char key[] = "temp";
      get_request(key);
      float temp;
      if(weather_response_buffer[0]!='\0') {
        temp = atof(weather_response_buffer);
      }
      else {
        temp=293.0;
      }
      return temp;
    }
  
};

#endif
