#include "Health.h"
#include <math.h>

#define BACKGROUND TFT_BLACK
#define FONTSIZE 1

Health_Module::Health_Module(TFT_eSPI* tft_to_use,int measurementPin) {
  tft=tft_to_use;
  is_active=false;
  measurement_Pin=measurementPin;
}

void Health_Module::activate() {
  state=-1;
  number_cross_threshold=0;
  is_active=true;
  is_increasing=true;
  tft->fillRect(0,20,130,140,BACKGROUND);
  tft->setCursor(0,20,FONTSIZE);
  tft->println("Please put your finger on the diode and press the button on the oximeter.");
  tft->println("");
  tft->println("Or please press on the diode if haven't already.");
}

void Health_Module::close() {
  is_active=false;
}
void Health_Module::update(int bv) {
  if(is_active) {
    raw_data=analogRead(measurement_Pin);
    averaged_data=(9*averaged_data+raw_data)/10.0;
    baseline=(49*baseline+averaged_data)/50.0;
    variance=(49*variance+(averaged_data-baseline)*(averaged_data-baseline))/50.0;
    std=sqrt(variance);
    deviation=averaged_data-baseline;
    if(state==-1) {
      if(averaged_data>500) {
        state=1;
        last_state_change_timer=millis();
        tft->fillRect(0,20,130,140,BACKGROUND);
        tft->setCursor(0,20,FONTSIZE);
        tft->print("Preparing");
      }
      /*
      Serial.print(deviation);
      Serial.print('\t');
      Serial.print(std);
      Serial.print('\t');
      Serial.print(0);
      Serial.print('\n');*/
    }
    
    else if(state==0) { //lower limit state
      if(raw_data!=0) {
        state=1;
        last_state_change_timer=millis();
        tft->fillRect(0,20,130,140,BACKGROUND);
        tft->setCursor(0,20,FONTSIZE);
        tft->print("Preparing");
      }
      /*
      Serial.print(deviation);
      Serial.print('\t');
      Serial.print(std);
      Serial.print('\t');
      Serial.print(0);
      Serial.print('\n');*/
    }
    else if(state==1) { //waiting state
      if(millis()-last_state_change_timer>100) {
        state=2;
        number_cross_threshold=0;
        variance=500*500;
        max=averaged_data;
        last_state_change_timer=millis();
        tft->fillRect(0,20,130,140,BACKGROUND);
        tft->setCursor(0,20,FONTSIZE);
        tft->print("Measuring");
      }
      else if(raw_data==0) {
        state=0;
        last_state_change_timer=millis();
        tft->fillRect(0,20,130,140,BACKGROUND);
        tft->setCursor(0,20,FONTSIZE);
        tft->print("Please press lighter");
      }
      else if(raw_data==4095) {
        state=3;
        last_state_change_timer=millis();
        tft->fillRect(0,20,130,140,BACKGROUND);
        tft->setCursor(0,20,FONTSIZE);
        tft->print("Please press harder");
      }
      /*
      Serial.print(deviation);
      Serial.print('\t');
      Serial.print(std);
      Serial.print('\t');
      Serial.print(0);
      Serial.print('\n');*/
    }
    else if(state==3) { //higher limit state
      if(raw_data!=4095){
        state=1;
        last_state_change_timer=millis();
        tft->fillRect(0,20,130,140,BACKGROUND);
        tft->setCursor(0,20,FONTSIZE);
        tft->print("Preparing");
      }
      /*
      Serial.print(deviation);
      Serial.print('\t');
      Serial.print(std);
      Serial.print('\t');
      Serial.print(0);
      Serial.print('\n');*/
    }
    else if(state==2) { //measuring state   
      if(raw_data==0||raw_data==4095) {
        state=5;
        last_state_change_timer=millis();
        /*
        Serial.print(deviation);
        Serial.print('\t');
        Serial.print(std);
        Serial.print('\t');
        Serial.print(0);
        Serial.print('\n');*/
      }
      else {
        if(is_increasing) {
          if(averaged_data>max) {
            max=averaged_data;
            /*
            Serial.print(deviation);
            Serial.print('\t');
            Serial.print(std);
            Serial.print('\t');
            Serial.print(0);
            Serial.print('\n');*/
          }
          else if(max-averaged_data>=std) {
            is_increasing=false;
            min=averaged_data;
            number_cross_threshold++;
            if(number_cross_threshold==10) {
              end_time=millis();
              state=4;
              last_state_change_timer=millis();
              tft->fillRect(0,20,130,140,BACKGROUND);
              tft->setCursor(0,20,FONTSIZE);
              tft->println("Your heartbeat:");
              tft->println(180000/(end_time-start_time));
              tft->print("Short press to measure again");
            }
            else if(number_cross_threshold==4) {
              start_time=millis();
            }
            /*
            Serial.print(deviation);
            Serial.print('\t');
            Serial.print(std);
            Serial.print('\t');
            Serial.print(1000);
            Serial.print('\n');*/
          }
          else {
            /*
            Serial.print(deviation);
            Serial.print('\t');
            Serial.print(std);
            Serial.print('\t');
            Serial.print(0);
            Serial.print('\n');*/
          }
        }
        else {
          if(averaged_data<min) {
            min=averaged_data;
            /*
            Serial.print(deviation);
            Serial.print('\t');
            Serial.print(std);
            Serial.print('\t');
            Serial.print(0);
            Serial.print('\n');*/
          }
          else if(averaged_data-min>=std) {
            is_increasing=true;
            max=averaged_data;
            number_cross_threshold++;
            if(number_cross_threshold==10) {
              end_time=millis();
              state=4;
              last_state_change_timer=millis();
              tft->fillRect(0,20,130,140,BACKGROUND);
              tft->setCursor(0,20,FONTSIZE);
              tft->println("Your heartbeat:");
              tft->println(180000/(end_time-start_time));
              tft->println("");
              tft->print("Short press the upper button to measure again");
            }
            else if(number_cross_threshold==4) {
              start_time=millis();
            }
            /*
            Serial.print(deviation);
            Serial.print('\t');
            Serial.print(std);
            Serial.print('\t');
            Serial.print(1000);
            Serial.print('\n');*/
          }
          else {
            /*
            Serial.print(deviation);
            Serial.print('\t');
            Serial.print(std);
            Serial.print('\t');
            Serial.print(0);
            Serial.print('\n');*/
          }
        }
      }
    }

    else if(state==5) { //another waiting state  
      if(raw_data<=30) {
        if(millis()-last_state_change_timer>500) {
          state=0;
          last_state_change_timer=millis();
          tft->fillRect(0,20,130,140,BACKGROUND);
          tft->setCursor(0,20,FONTSIZE);
          tft->print("Please press lighter");
        }
      }
      else if(raw_data>=4065) {
        if(millis()-last_state_change_timer>500) {
          state=3;
          last_state_change_timer=millis();
          tft->fillRect(0,20,130,140,BACKGROUND);
          tft->setCursor(0,20,FONTSIZE);
          tft->print("Please press harder");
        }
      }
      else {
        state=2;
        last_state_change_timer=millis();
      }
      /*
      Serial.print(deviation);
      Serial.print('\t');
      Serial.print(std);
      Serial.print('\t');
      Serial.print(0);
      Serial.print('\n');*/
    }
    else if(state==4) {
        if(bv==1) {
          activate();
        }
        /*
        Serial.print(deviation);
        Serial.print('\t');
        Serial.print(std);
        Serial.print('\t');
        Serial.print(0);
        Serial.print('\n');*/
    }
  }
}