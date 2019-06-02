#ifndef Time_H
#define Time_H

#include "Arduino.h"
#include "Requests.h"
#include <TinyGPS++.h>

#define ANALOG 2  //example definition
#define DIGITAL 3  //example 
#define STOPWATCH 4  //example 

class Time {
    uint8_t mins;
    float minsd;
    uint8_t secs;
    float hoursd;
    float old_hoursd;
    float old_minsd;
    uint8_t old_secs;
    uint8_t hours;
    int lat_deg; //degrees portion of lattitude
    float lat_dm; //latitude decimal minutes
    char lat_dir; //latitude direction
    int lon_deg; //longitude in degrees
    float lon_dm; //longitude decimal minutes
    char lon_dir; //longitude direction
    float sign_lat_deg; //signed value of latitude (minutes are decimals)
    float sign_lon_deg; //signed value of longtitude (minutes are decimals)
    TFT_eSPI* tft;
    const uint16_t WAIT_PERIOD = 4000; //how much to wait for GPS signal
    bool update_date = true;
    const int LOOP_PERIOD = 1000; //periodicity of getting the time.
    char time_buffer[200] = {'\0'}; //dumping chars into
    char time_request_buffer[1000]; //char array buffer to hold HTTP request
    char time_response_buffer[1000]; //char array buffer to hold HTTP response
    const uint16_t TIME_IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
    const uint16_t TIME_OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
    const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
    const int CLOCK_BUFFER_LENGTH = 200;  //size of char array we'll use for
    TinyGPSPlus* gps; //instantiate approporiate Serial object for GPS

  public:
    bool do_not_request = false;
    char curtime[20];//pointer to the location of the char array with current time
    char curdate[20];//pointer to the location of the char array with current date
    int year = 2000; //year
    int month = 1; //month
    int day = 1; //day of month
    int hour = 0; //hour (24 clock GMT)
    int minute = 0; //minute
    int second = 0; //second
    int center; //the coordinates of the center of the analog watchface
    int hourl; //length of the hour hand
    int minl; //length of the min hand
    int secl; //length of the sec hand
    bool prepare_analog = false;
    unsigned long clock_timer;  //used for storing millis() readings.
    unsigned long gps_timer;  //used for storing millis() readings.
    uint8_t display_state;  //system state (feel free to use
    const int GETTING_PERIOD = 60000; //periodicity of getting the time.
    uint8_t time_button_state; //state of the button
    uint32_t stopwatch_timer;
    uint8_t stopwatch_sec = 0;
    uint8_t stopwatch_min = 0;
    bool stopwatch_running = false;

    Time (TFT_eSPI* tft_to_use, TinyGPSPlus* gps_to_use, int in_hourl, int in_minl, int in_secl, int in_center) {
      center = in_center;
      hourl = in_hourl;
      minl = in_minl;
      secl = in_secl;
      tft = tft_to_use;
      gps = gps_to_use;
      start();
      get_time();
      get_date();
      Serial.println("test1");
      clock_timer = millis();
      display_state = ANALOG;
      prepare_analog = true;
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

    void print_analog() {
      tft->fillRect(0, 20, 130, 140, TFT_BLACK);
      tft->drawCircle(center, center, center - 20, TFT_GREEN);

      coverline(center, 30 * old_hoursd, hourl);
      coverline(center, 6 * old_minsd, minl);
      coverline(center, 6 * old_secs, secl);

      drawline(center, 30 * hoursd, hourl);
      drawline(center, 6 * minsd, minl);
      drawline(center, 6 * secs, secl);
      tft->setCursor(0, 120, 4); //set cursor at bottom of screen, and set font size to 4
      if (update_date)  tft->println(curdate); //print a whole line.
      tft->setTextSize(1);
    }

    void drawline(int center, double ang, int l) {
      ang = (ang * M_PI) / 180;
      tft->drawLine(center, center, center + sin(ang) * l, center - cos(ang) * l, TFT_GREEN);
    }

    void coverline(int center, double ang, int l) {
      ang = (ang * M_PI) / 180;
      tft->drawLine(center, center, center + sin(ang) * l, center - cos(ang) * l, TFT_BLACK);
    }

    void print_digital() {
      tft->fillRect(0, 20, 130, 140, TFT_BLACK);
      tft->setCursor(0, 60, 4); //set cursor at bottom of screen, and set font size to 4
      tft->println(curtime); //print a whole line.
      if (update_date)  tft->println(curdate); //print a whole line.
      tft->setTextSize(1);
    }

    void stopwatch() {
      if (stopwatch_running){
        if (millis() - stopwatch_timer >= 1000) {
          stopwatch_sec ++;
          stopwatch_timer = millis();
          if (stopwatch_sec == 60) {
            stopwatch_sec = 0;
            stopwatch_min ++;
          }
        }
        tft->fillRect(0, 20, 130, 140, TFT_BLACK);
        tft->setCursor(0, 60, 4); //set cursor at bottom of screen, and set font size to 4
        char info[14];
        Serial.println(millis() - stopwatch_timer);
        sprintf(info, "%02d:%02d.%02d", stopwatch_min, stopwatch_sec, int((millis() - stopwatch_timer) / 10.0));
        tft->println(info); //print a whole line.
        tft->setTextSize(1);
      }
    }

    void change_time() {
      if (gps->location.isValid()) {
        sign_lon_deg =  gps->location.lng();
        sign_lat_deg =  gps->location.lat();
      }
      if (millis() - clock_timer >= LOOP_PERIOD) {
        curtime[7] ++;
        if (curtime[7] == ':') {
          curtime[7] = '0';
          curtime[6] ++;
          if (curtime[6] == '6') {
            curtime[6] = '0';
            curtime[4] ++;
            if (curtime[4] == ':') {
              curtime[4] = '0';
              curtime[3]++;
              if (curtime[3] == '6') {
                curtime[1] ++;
                curtime[3] = '0';
                if (curtime[1] == ':') {
                  curtime[1] = '0';
                  curtime[0] ++;
                }
                if (curtime[1] == '4' && curtime[0] == '2') {
                  curtime[1] = '0';
                  curtime[0] = '0';
                  day ++;
                  if ((month == 11 || month == 9 || month == 6 || month == 4) && day == 31) {
                    day = 1;
                    month ++;
                  }
                  if ((month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 ) && day == 32) {
                    day = 1;
                    month ++;
                  }

                  if ((month == 12) && day == 32) {
                    update_date = true;
                    day = 1;
                    month = 1;
                    year ++;
                  }

                  if ((month == 2) && day == 29 && year % 4 != 0) {
                    day = 1;
                    month ++;

                  }
                  if ((month == 2) && day == 30 && year % 4 == 0) {
                    day = 1;
                    month ++;
                  }
                }
              }
            }
          }
        }
        const char* del = ":";

        char *tok;
        char timecopy[20];
        strcpy(timecopy, curtime);

        tok = strtok(timecopy, del);
        hours = atoi(tok);
        tok = strtok(NULL, del);
        mins = atoi(tok);
        tok = strtok(NULL, del);
        old_secs = secs;
        secs = atoi(tok);
        hour = hours;
        minute = mins;
        second = secs;

        old_minsd = minsd;
        old_hoursd = hoursd;
        minsd = mins + secs / 60.;
        hoursd = hours + mins / 60.;

        sprintf(curdate, "%04d-%02d-%02d", year, month, day);

        clock_timer = millis();
      }
    }

    void display() {
      if (millis() - clock_timer >= LOOP_PERIOD) {
        if (display_state == ANALOG) print_analog();
        else if (display_state == DIGITAL) print_digital();
        else stopwatch();
      }
    }

    void get_time() { //for getting the time info
      tft->setCursor(0, 10, 1);
      char key[] = "time";
      get_request(key);

      Serial.println(time_response_buffer);
      if (time_response_buffer[0] != '\0') {
        char *timep;
        timep = time_response_buffer;

        char str1[3] = {*timep, *(timep + 1)};
        hour = atoi(str1);

        char str2[3] = {*(timep + 3), *(timep + 4)};
        minute = atoi(str2);


        char str3[3] = {*(timep + 6), *(timep + 7)};
        second = atoi(str3);
      }
      char info[200] = {0};
      sprintf(info, "%02d:%02d:%02d", hour, minute, second);
      strcpy(curtime, info);
    }

    void get_request(char * key) { //request to 608dev
      memset(time_request_buffer, '\0', sizeof(time_request_buffer));
      sprintf(time_request_buffer, "GET http://608dev.net/sandbox/sc/asselism/weather.py?lat=%2.4f&lon=%3.4f&key=%s HTTP/1.1\r\n", sign_lat_deg, sign_lon_deg, key);
      strcat(time_request_buffer, "Host: 608dev.net\r\n"); //add more to the end
      strcat(time_request_buffer, "\r\n"); //add blank line!
      Serial.println(time_request_buffer);
      do_http_request("608dev.net", time_request_buffer, time_response_buffer, TIME_OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
      Serial.println(time_response_buffer);
    }

    void get_date() { //for getting the date info
      tft->setCursor(0, 10, 1);
      char key[] = "date";
      get_request(key);
      if (time_response_buffer[0] != '\0') {
        char *date = time_response_buffer;

        char str8[5] = {*(date), *(date + 1), *(date + 2), *(date + 3)};
        year = atoi(str8);

        char str9[3] = {*(date + 5), *(date + 6)};
        month = atoi(str9);

        char str10[3] = {*(date + 8), *(date + 9)};
        day = atoi(str10);
      }
      char info[200] = {0};
      sprintf(info, "%04d-%02d-%02d", year, month, day);
      strcpy(curdate, info);
    }
};

#endif
