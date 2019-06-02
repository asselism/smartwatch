#include "Arduino.h"
#include "icons.h"

//Battery icon
Battery_icon::Battery_icon(TFT_eSPI* tft, uint8_t x_loc, uint8_t y_loc, uint16_t color, uint16_t background, uint16_t text_c) {
  screen = tft;
  x = x_loc;
  y = y_loc;
  fc = color;
  bgc = background;
  tc = text_c;
  last_update = millis()-500;
}

double Battery_icon::voltage_from_discharge(double discharge) {
  return ((((-5.08321149*discharge)+8.16962756)*discharge-3.53049671)*discharge-0.3295403)*discharge+4.08151442;
}

double Battery_icon::discharge_from_voltage(double voltage, double error) {
  double a=0;
  double b=1;
  int i=0;
  if(voltage_from_discharge(a)<voltage) {
    return a;
  }
  if(voltage_from_discharge(b)>voltage) {
    return b;
  }
  while(i<10000) {
    if(voltage_from_discharge((a+b)/2)==voltage) {
      return (a+b)/2;
    }
    else if(voltage_from_discharge((a+b)/2)>voltage) {
      a=(a+b)/2;
    }
    else {
      b=(a+b)/2;
    }
    if(fabs(a-b)<error) {
      return (a+b)/2;
    }
    i++;
  }
  return (a+b)/2;
}

void Battery_icon::drawBattery(float level) {
  screen->fillRect(x, y, 20, 8, bgc);
  screen->drawRect(x, y, 20, 8, fc);
  screen->fillRect(x, y, 20*level, 8, fc);
  screen->fillRect(x+20, y+2, 2, 4, fc);
}

void Battery_icon::update() {
  if(millis()-last_update>500||millis()<last_update||need_to_draw_immediately) {
    double voltage = analogRead(A6) * 3.3 / 4096 * 2; // get Battery Voltage (mV)
    double discharge_amt = discharge_from_voltage(voltage, 0.001); //get discharge amount using battery voltage
    drawBattery(1.0 - discharge_amt);
    if (is_in_home_page) {
      char info[6];
      int time = round(10.0*(1.0 - discharge_amt));
      sprintf(info,"~%2dh",time);
      screen->fillRect(94,11,32,11,bgc);
      screen->setCursor(101,13,1);
      screen->setTextColor(fc);
      screen->print(info);
      screen->setTextColor(tc, bgc);
    }
    last_update=millis();
    need_to_draw_immediately=false;
  }
}

//WiFi icon
WiFi_icon::WiFi_icon(TFT_eSPI* tft, uint8_t x_loc, uint8_t y_loc, uint16_t color_on, uint16_t color_off, uint16_t bg_c) {
  screen = tft;
  x = x_loc;
  y = y_loc;
  c_on = color_on;
  c_off = color_off;
  bc = bg_c;
  status = 0;
}

void WiFi_icon::drawWiFi() {
  uint16_t fc;
  if (status) {
    fc = c_on;
  } else {
    fc = c_off;
  }
  screen->drawCircle(x, y, 8, fc);
  screen->drawCircle(x, y, 6, fc);
  screen->drawCircle(x, y, 4, fc);
  screen->fillCircle(x, y, 2, fc);
  screen->fillRect(x - 8, y - 8, 8, 16, bc);
  screen->fillRect(x - 2, y + 1, 11, 10, bc);
}

//GPS icon
GPS_icon::GPS_icon(TinyGPSPlus* GPS_to_use, TFT_eSPI* tft, uint8_t x_loc, uint8_t y_loc, uint16_t color_on, uint16_t color_off, uint16_t text_c) {
  gps = GPS_to_use;
  screen = tft;
  x = x_loc;
  y = y_loc;
  c_on = color_on;
  c_off = color_off;
  tc = text_c;

  status = 0;
  old_status = 0;
}

void GPS_icon::get_GPS_status() {
  if(gps->location.isValid())
  {
    status=1;
  }
  else
  {
    status=0;
  }
}

void GPS_icon::drawGPS() {
  if (status) {
    screen->setTextColor(c_on);
    screen->drawString("GPS", x, y, 1);
  } else {
    screen->setTextColor(c_off);
    screen->drawString("GPS", x, y, 1);
  }
  screen->setTextColor(tc);
}

void GPS_icon::update() {
  get_GPS_status();
  if (status != old_status) {
    old_status = status;
    drawGPS();
  }
}