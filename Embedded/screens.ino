#include <TinyGPS++.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <WiFi.h> //Connect to WiFi Network
#include <mpu9255_esp32.h>

#include "Button.h"
#include "icons.h"
#include "Time.h"
#include "Weather.h"
#include "Steps.h"
#include "Health.h"
#include "Speech_search.h"
#include "Sleep.h"
#include "Friends.h"

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
HardwareSerial gps_serial(2); //instantiate approporiate Serial object for GPS
TinyGPSPlus gps;

//Wifi info
char user[10] = "assel";
/*
char network[] = "MIT";  //SSID for 6.08 Lab
char password[] = ""; //Password for 6.08 Lab
*/
char network[] = "6s08";  //SSID for 6.08 Lab
char password[] = ""; //Password for 6.08 Lab

//Loop Timing Variables
const uint32_t LOOP_PERIOD = 10; //(ms)..how quickly primary loop iterates
uint32_t loop_timer; //used for timing loop
uint32_t* sleep_timer;
bool is_wifi_connected;

int hour = 0;
int minute = 0;
int second = 0;
int year = 0;
int month = 0;
int day = 0;
int steps = 0;
int old_minute = 0;
int old_second = 0;
int old_steps = 0;
bool need_to_draw_immediately = true;

//Button states
#define UNPRESSED 1
#define PRESSED 0
#define ANALOG 2  //example definition
#define DIGITAL 3  //example 
#define STOPWATCH 4
uint8_t time_button_state; //state of the button

//Color settings
const int BACKGROUND = TFT_BLACK;
const int TEXT_COLOR = TFT_GREEN;
const int BATT_COLOR = TFT_GREEN;
const int BATT_LOW_COLOR = TFT_YELLOW;
const int GPS_ON_COLOR = TFT_RED;
const int GPS_OFF_COLOR = 0x8410;
const int WIFI_ON_COLOR = TFT_BLUE;
const int WIFI_OFF_COLOR = 0x8410; // gray
const int SELECT_COLOR = TFT_BLUE;
const int BACK_ARROW = TFT_BLUE;

//initial values
uint8_t screen_state = 0; //default to home screen
uint8_t select_item = 0; // select nothing by default
uint8_t app;

const int INACTIVE_TIME = 30000;

//static temp info
char weather[8] = "Sunny";
char temperature[4] = "20";
int temp;

//create objects
const uint8_t PIN_1 = 16; //pin connected to button1
const uint8_t PIN_2 = 5; //pin connected to button2
const uint8_t output_pin = 12; //for controlling LCD sleep

Button btn16(PIN_1);
Button btn5(PIN_2);
MPU9255 imu;

Battery_icon batt(&tft, 104, 3, BATT_LOW_COLOR, BACKGROUND, TEXT_COLOR);
WiFi_icon wifi_icon(&tft, 73, 10, WIFI_ON_COLOR, WIFI_OFF_COLOR, BACKGROUND);
GPS_icon GPS_obj(&gps, &tft, 85, 3, GPS_ON_COLOR, GPS_OFF_COLOR, TEXT_COLOR);

Sleep auto_sleep(&tft, output_pin, &screen_state, BACKGROUND, TEXT_COLOR, SELECT_COLOR, BACK_ARROW);

Time timeobj(&tft, &gps, 64 - 40, 64 - 30, 64 - 25, 64);
Weather weatherobj(&tft, &gps);
Steps_Module steps_module(&tft, &imu, user, &timeobj);
Health_Module health_module(&tft, A7);
Wiki_search wikiobj(&tft, PIN_2);
Friends_Module friends_module(&tft, &gps, &imu, user);

void draw_weather(TFT_eSPI* screen, char* wea, char* temp, uint8_t y, uint16_t TEXT_COLOR) {
  screen->drawString(wea, 21, y, 2);
  screen->drawString(temp, 79, y, 2);
  screen->drawString("C", 104, y, 2);
  screen->drawCircle(99, y + 5, 2, TEXT_COLOR);
}

void drawMenu() {
  tft.drawLine(0, 13, 127, 13, TEXT_COLOR);
  tft.drawString("Time", 5, 20, 1);
  tft.drawLine(0, 33, 127, 33, TEXT_COLOR);
  tft.drawString("Search", 5, 40, 1);
  tft.drawLine(0, 53, 127, 53, TEXT_COLOR);
  tft.drawString("Step", 5, 60, 1);
  tft.drawLine(0, 73, 127, 73, TEXT_COLOR);
  tft.drawString("Health", 5, 80, 1);
  tft.drawLine(0, 93, 127, 93, TEXT_COLOR);
  tft.drawString("Friends", 5, 100, 1);
  tft.drawLine(0, 113, 127, 113, TEXT_COLOR);
  tft.drawString("Auto Sleep", 5, 120, 1);
  tft.drawLine(0, 133, 127, 133, TEXT_COLOR);
  tft.drawTriangle(6, 147, 19, 141, 19, 153, BACK_ARROW);
};

void draw_Friends_Menu() {
  tft.drawLine(0, 13, 127, 13, TEXT_COLOR);
  tft.drawString("Add", 5, 20, 1);
  tft.drawLine(0, 33, 127, 33, TEXT_COLOR);
  tft.drawString("Message", 5, 40, 1);
  tft.drawLine(0, 53, 127, 53, TEXT_COLOR);
  tft.drawString("Nearby", 5, 60, 1);
  tft.drawLine(0, 73, 127, 73, TEXT_COLOR);
  tft.drawTriangle(6, 147, 19, 141, 19, 153, BACK_ARROW);
};

void drawTopBar() { //initialize top bar: battery, wifi, and GPS (no line, no time)
  GPS_obj.get_GPS_status();
  GPS_obj.drawGPS();
  batt.update();
  wifi_icon.drawWiFi();
}

void updateTopBar() {
  batt.update(); // update the battery icon
  GPS_obj.update(); // update the GPS icon when the connection changes
  if (screen_state == 0) { //if main screen, don't need to draw extra
    if (WiFi.status() == WL_CONNECTED) { //change wifi to blue if reconnected
      if (!wifi_icon.status) {
        wifi_icon.status = 1;
        wifi_icon.drawWiFi();
      }
    } else { //change wifi to gray if disconnected
      if (wifi_icon.status) {
        wifi_icon.status = 0;
        wifi_icon.drawWiFi();
      }
    }
  } else { // if in menu or app
    if (old_minute != minute || need_to_draw_immediately)
    {
      char time_info[30];
      sprintf(time_info, "%02d:%02d", hour,  minute);
      tft.fillRect(0, 0, 50, 10, TFT_BLACK);
      tft.drawString(time_info, 3, 3, 1);        // needs to update to pointer form, and make it update only when time changes
      need_to_draw_immediately = false;
    }
    if (WiFi.status() == WL_CONNECTED) { //change wifi to blue if reconnected
      if (!wifi_icon.status) {
        wifi_icon.status = 1;
        wifi_icon.drawWiFi();
        tft.drawLine(0, 13, 127, 13, TEXT_COLOR);
      }
    } else { //change wifi to gray if disconnected
      if (wifi_icon.status) {
        wifi_icon.status = 0;
        wifi_icon.drawWiFi();
        tft.drawLine(0, 13, 127, 13, TEXT_COLOR);
      }
    }
  }
}

void time_button_switch(uint8_t input) {
  if (input == 2 && time_button_state == UNPRESSED) time_button_state = PRESSED;
  else if (!input && time_button_state == PRESSED) {
    Serial.println("Hi");
    Serial.println(timeobj.display_state);
    time_button_state = UNPRESSED;
    switch (timeobj.display_state) {
      case ANALOG:
        Serial.println("Analog");
        timeobj.display_state =  DIGITAL;
        timeobj.print_analog();
        break;
      case DIGITAL:
        Serial.println("Digital");
        timeobj.display_state = STOPWATCH;
        tft.fillRect(0,20,130,140,TFT_BLACK);
        tft.setCursor(0, 60, 4); //set cursor at bottom of screen, and set font size to 4
        char info[14];
        sprintf(info, "%02d:%02d.%02d", 0, 0, 0);
        tft.println(info); //print a whole line.
        tft.setTextSize(1);
        break;

      case STOPWATCH:
        Serial.println("Stopwatch");
        timeobj.display_state = ANALOG;
        timeobj.prepare_analog = true;
        timeobj.stopwatch_sec = 0;
        timeobj.stopwatch_min = 0;
        break;
    }
  }

  else if (input == 1 && timeobj.display_state == STOPWATCH) {
    if (!timeobj.stopwatch_running){
      timeobj.stopwatch_running = true;
    timeobj.stopwatch();
    timeobj.stopwatch_timer = millis();

  }
  else timeobj.stopwatch_running = false;
  }
}

void setup() {
  Serial.begin(115200);  // Set up serial port
  pinMode(PIN_1, INPUT_PULLUP);
  pinMode(PIN_2, INPUT_PULLUP);
  pinMode(output_pin, OUTPUT);
  digitalWrite(output_pin, HIGH);
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.fillScreen(BACKGROUND); //fill background
  tft.setTextColor(TEXT_COLOR, BACKGROUND); //set color of font to green foreground, black background
  gps_serial.begin(9600, SERIAL_8N1, 32, 33);
  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(300);
    Serial.print(".");
    count++;
  }
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    wifi_icon.status = 1;
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    //ESP.restart(); // restart the ESP (proper way)
  }
  if (imu.setupIMU(1)) {
    Serial.println("IMU Connected!");
  } else {
    Serial.println("IMU Not Connected :/");
    Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }

  time_button_state = UNPRESSED; //button is not pressed
  timeobj.start();

  timeobj.get_time();
  timeobj.get_date();
  temp = weatherobj.get_temp();
  sprintf(temperature, "%2d", temp - 273);

  weatherobj.get_vis();
  strcpy(weather, weatherobj.weather_response_buffer);

  drawTopBar();
  loop_timer = millis();
  sleep_timer = &(auto_sleep.timer);
  *sleep_timer = millis();
}

void loop() {
  is_wifi_connected = (WiFi.status() == WL_CONNECTED);
  hour = timeobj.hour;
  minute = timeobj.minute;
  second = timeobj.second;
  year = timeobj.year;
  month = timeobj.month;
  day = timeobj.day;
  steps = steps_module.get_steps();
  int bv_1 = btn16.update(); //get button value
  int bv_2 = btn5.update();
  
  if (auto_sleep.state == 0) {
    switch (screen_state) {
      case 0: // home screen
        updateTopBar();
        draw_weather(&tft, weather, temperature, 27, TEXT_COLOR);
        if (old_minute != minute || old_steps != steps || need_to_draw_immediately)
        {
          char time_info[30];
          sprintf(time_info, "%02d:%02d", hour,  minute);
          char date_info[30];
          sprintf(date_info, "%04d-%02d-%02d", year,  month,  day);
          char steps_info[30];
          sprintf(steps_info, "Today's steps: %d", steps);
          tft.fillRect(0, 42, 130, 140, TFT_BLACK);
          tft.setCursor(0, 0, 1);
          tft.setTextSize(3);
          tft.drawString(time_info, 22, 53);
          tft.setTextSize(1);
          tft.drawString(date_info, 28, 87, 2);
          tft.drawString(steps_info, 8, 130, 1);
          need_to_draw_immediately = false;
        }

        if (bv_1 != 0 || bv_2 != 0) { // short press switchs to menu state
          screen_state = 1;
          tft.fillScreen(BACKGROUND); //fill background
          drawTopBar();
          drawMenu();
          need_to_draw_immediately = true;
          batt.need_to_draw_immediately = true;
          batt.is_in_home_page = false;
        }
        break;

      case 1: // menu
        if (bv_1 == 1) {
          select_item++;
        }
        updateTopBar();
        switch (select_item) {
          case 0: //nothing selected
            break;
          case 1: //app 1: Time
            tft.fillRect(105, 18, 10, 10, SELECT_COLOR);
            if (bv_1 == 2) {
              select_item = 0;
              app = 1; // enter app 1
              screen_state = 2;
              tft.fillScreen(BACKGROUND); //fill background
              drawTopBar();
              need_to_draw_immediately = true;
              batt.need_to_draw_immediately = true;
            }
            break;
          case 2: //app 2: Search
            tft.fillRect(105, 18, 10, 10, BACKGROUND);
            tft.fillRect(105, 38, 10, 10, SELECT_COLOR);
            if (bv_1 == 2) {
              select_item = 0;
              app = 2; // enter app 2
              screen_state = 2;
              tft.fillScreen(BACKGROUND); //fill background
              drawTopBar();
              wikiobj.start();
              need_to_draw_immediately = true;
              batt.need_to_draw_immediately = true;
            }
            break;
          case 3: //app 3: Step
            tft.fillRect(105, 38, 10, 10, BACKGROUND);
            tft.fillRect(105, 58, 10, 10, SELECT_COLOR);
            if (bv_1 == 2) {
              select_item = 0;
              app = 3; // enter app 3
              screen_state = 2;
              tft.fillScreen(BACKGROUND); //fill background
              drawTopBar();
              steps_module.activate();
              need_to_draw_immediately = true;
              batt.need_to_draw_immediately = true;
            }
            break;
          case 4: //app 4: Health
            tft.fillRect(105, 58, 10, 10, BACKGROUND);
            tft.fillRect(105, 78, 10, 10, SELECT_COLOR);
            if (bv_1 == 2) {
              select_item = 0;
              app = 4; // enter app 4
              screen_state = 2;
              tft.fillScreen(BACKGROUND); //fill background
              drawTopBar();
              health_module.activate();
              timeobj.do_not_request = true;
              steps_module.do_not_request = true;
              friends_module.add_module.do_not_request = true;
              friends_module.message_module.do_not_request = true;
              friends_module.nearby_module.do_not_request = true;
              need_to_draw_immediately = true;
              batt.need_to_draw_immediately = true;
            }
            break;
          case 5: //app 5: Friends
            tft.fillRect(105, 78, 10, 10, BACKGROUND);
            tft.fillRect(105, 98, 10, 10, SELECT_COLOR);
            if (bv_1 == 2) {
              select_item = 9;
              tft.fillScreen(BACKGROUND); //fill background
              drawTopBar();
              draw_Friends_Menu();
              need_to_draw_immediately = true;
              batt.need_to_draw_immediately = true;
            }
            break;
          case 6: //app 6: Auto Sleep
            tft.fillRect(105, 98, 10, 10, BACKGROUND);
            tft.fillRect(105, 118, 10, 10, SELECT_COLOR);
            if (bv_1 == 2) {
              select_item = 0;
              app = 6; // enter app 6
              screen_state = 2;
              tft.fillScreen(BACKGROUND); //fill background
              drawTopBar();
              auto_sleep.drawChoice();
              need_to_draw_immediately = true;
              batt.need_to_draw_immediately = true;
            }
            break;
          case 7: //back to home button selection
            tft.fillRect(105, 118, 10, 10, BACKGROUND);
            tft.fillTriangle(6, 147, 19, 141, 19, 153, BACK_ARROW);
            if (bv_1 == 2) {
              select_item = 0;
              screen_state = 0; // back to home screen
              tft.fillScreen(BACKGROUND); //fill background
              drawTopBar();
              need_to_draw_immediately = true;
              batt.need_to_draw_immediately = true;
              batt.is_in_home_page = true;
            }
            break;
          case 8:
            tft.fillRect(6, 141, 14, 13, BACKGROUND);
            tft.drawTriangle(6, 147, 19, 141, 19, 153, BACK_ARROW);
            select_item = 0;
            break;
          case 9://nothing selected in friends
            break;
          case 10://add selected in friends
            tft.fillRect(105, 18, 10, 10, SELECT_COLOR);
            if (bv_1 == 2) {
              select_item = 9;
              app = 10; // enter app 10
              screen_state = 2;
              tft.fillScreen(BACKGROUND); //fill background
              drawTopBar();
              friends_module.activate(4);
              need_to_draw_immediately = true;
              batt.need_to_draw_immediately = true;
            }
            break;
          case 11://message selected in friends
            tft.fillRect(105, 18, 10, 10, BACKGROUND);
            tft.fillRect(105, 38, 10, 10, SELECT_COLOR);
            if (bv_1 == 2) {
              select_item = 9;
              app = 11; // enter app 10
              screen_state = 2;
              tft.fillScreen(BACKGROUND); //fill background
              drawTopBar();
              friends_module.activate(1);
              need_to_draw_immediately = true;
              batt.need_to_draw_immediately = true;
            }
            break;
          case 12://nearby selected in friends
            tft.fillRect(105, 38, 10, 10, BACKGROUND);
            tft.fillRect(105, 58, 10, 10, SELECT_COLOR);
            if (bv_1 == 2) {
              select_item = 9;
              app = 12; // enter app 10
              screen_state = 2;
              tft.fillScreen(BACKGROUND); //fill background
              drawTopBar();
              friends_module.activate(3);
              need_to_draw_immediately = true;
              batt.need_to_draw_immediately = true;
            }
            break;
          case 13://back to main menu selected in friends
            tft.fillRect(105, 58, 10, 10, BACKGROUND);
            tft.fillTriangle(6, 147, 19, 141, 19, 153, BACK_ARROW);
            if (bv_1 == 2) {
              select_item = 5;
              screen_state = 1; // back to home screen
              tft.fillScreen(BACKGROUND); //fill background
              drawTopBar();
              drawMenu();
              need_to_draw_immediately = true;
              batt.need_to_draw_immediately = true;
            }
            break;
          case 14:
            tft.fillRect(6, 141, 14, 13, BACKGROUND);
            tft.drawTriangle(6, 147, 19, 141, 19, 153, BACK_ARROW);
            select_item = 9;
            break;
          default:
            tft.drawString("menu's wrong", 5, 140, 1);
        }
        break;

      case 2: // app
        updateTopBar();
        switch (app) {
          case 1: //Time app
            timeobj.display();
            if (millis() - timeobj.clock_timer >= timeobj.GETTING_PERIOD && !timeobj.do_not_request && is_wifi_connected) {
              timeobj.get_time();
              timeobj.clock_timer = millis();
            }
            time_button_switch(bv_2); //Call our button every time through the loop
            break;
          case 2: //Search app
            wikiobj.update(digitalRead(5), is_wifi_connected);
            break;
          case 3: //Step app
            break;
          case 4: //Health app
            break;
          case 5:
            break;
          case 6:
            auto_sleep.set_fsm(bv_2);
            break;
          case 10:
            //      tft.drawString("> Add app", 5, 20, 1);
            break;
          case 11:
            //       tft.drawString("> Message app", 5, 20, 1);
            break;
          case 12:
            //       tft.drawString("> Nearby app", 5, 20, 1);
            break;
          default:
            tft.drawString("app's wrong", 5, 20, 1);
        }
        if (bv_1 == 2) {
          screen_state = 1; // back to menu
          tft.fillScreen(BACKGROUND); //fill background
          drawTopBar();
          if (select_item == 0)
          {
            drawMenu();
          }
          else if (select_item == 9)
          {
            draw_Friends_Menu();
          }
          need_to_draw_immediately = true;
          batt.need_to_draw_immediately = true;
          steps_module.close();
          health_module.close();
          friends_module.close();
          auto_sleep.set_state = 0;
          timeobj.do_not_request = false;
          steps_module.do_not_request = false;
          friends_module.add_module.do_not_request = false;
          friends_module.message_module.do_not_request = false;
          friends_module.nearby_module.do_not_request = false;
        }
        break;
      default:
        tft.drawString("screen's wrong", 5, 140, 1);
    }
  }

  auto_sleep.fsm(bv_1, bv_2); //detect if should sleep

  if (gps_serial.available()) {
    while (gps_serial.available())
      gps.encode(gps_serial.read());
  }
  timeobj.change_time();
  steps_module.update(bv_2, is_wifi_connected);
  health_module.update(bv_2);
  friends_module.update(bv_2, is_wifi_connected);

  old_minute = minute;
  old_second = second;
  old_steps = steps;

  while (millis() - loop_timer < LOOP_PERIOD);
  loop_timer = millis();
}
