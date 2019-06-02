#define BACKGROUND TFT_BLACK
#define LINECOLOR TFT_GREEN
#define FONTSIZE 1

#include "Steps.h"
#include "Requests.h"

Steps_Module::Steps_Module(TFT_eSPI* tft_to_use, MPU9255* imu_to_use, char* username, Time* time_module_to_use):type_module(message_to_type,imu_to_use){
  tft=tft_to_use;
  imu=imu_to_use;
  user=username;
  time_module=time_module_to_use;
  post_timer=millis();
}

void Steps_Module::initialize() {
  year=time_module->year;
  month=time_module->month;
  day=time_module->day;
  sprintf(request,"GET http://608dev.net/sandbox/sc/haoyangg/final_project/steps.py?user=%s&year=%d&month=%d&day=%d&action=initialize HTTP/1.1\r\n",user,year,month,day);
  strcat(request,"Host: 608dev.net\r\n");
  strcat(request,"\r\n");
  do_http_request("608dev.net", request, response, 1000, 6000,true);
  if(response[0]!='\0') {
    char* p=strtok(response,",");
    for(int i=0;i<10;i++) {
      history_data[i]=atoi(p);
      p=strtok(NULL,",");
    }
    steps=atoi(p);
  }
  else {
    for(int i=0;i<10;i++) {
      history_data[i]=0;
    }
    steps=0;
  }
  last_post_data=steps;
  is_initialized=true;
}

void Steps_Module::activate() {
  is_active=true;
  type_module.reset();
  draw();
}

void Steps_Module::draw() {
  tft->fillRect(0,20,130,140,BACKGROUND);
  tft->setCursor(0,20,FONTSIZE);
  tft->print("Today's steps: ");
  tft->print(steps);
  tft->setCursor(0,50,FONTSIZE);
  tft->print("10 days history:");
  tft->setCursor(0,140,FONTSIZE);
  tft->print("Long press to check other date");
  int scale=0;
  for(int i=0;i<10;i++) {
    if(history_data[i]>scale) {
      scale=history_data[i];
    }
  }
  scale++;
  tft->drawLine(10,130,125,130,LINECOLOR);
  tft->drawLine(20,140,20,70,LINECOLOR);
  tft->setCursor(0,127,FONTSIZE);
  tft->print(0);
  tft->setCursor(0,72,FONTSIZE);
  tft->print(scale);
  for(int i=0;i<9;i++) {
    tft->drawLine(30+10*i,130-history_data[i]*55/scale,40+10*i,130-history_data[i+1]*55/scale,LINECOLOR);
  }
  for(int i=0;i<10;i++) {
    tft->fillCircle(30+10*i,130-history_data[i]*55/scale,2,LINECOLOR);
  }
}

void Steps_Module::close() {
  state=0;
  is_active=false;
}

void Steps_Module::update(int bv, bool is_wifi_connected) {
  if(!is_initialized) {
    initialize();
  }
  else {
 //the code was omitted because this project was completed as part of
 //Embedded Systems (6.08) class. The current function was partly written
 //as part of a homework exercise and deleted as per class's sharing policy
    if((millis()-post_timer>30000||millis()<post_timer)&&state==0&&!do_not_request) {
      if(day!=time_module->day) {
        year=time_module->year;
        month=time_module->month;
        day=time_module->day;
        for(int i=0;i<9;i++) {
          history_data[i]=history_data[i+1];
        }
        history_data[10]=last_post_data;
        steps=0;
        last_post_data=0;
        if(is_active) {
        draw();
        }
      }
      sprintf(body,"user=%s&year=%d&month=%d&day=%d&steps=%d",user,year,month,day,steps);
      sprintf(request,"POST http://608dev.net/sandbox/sc/haoyangg/final_project/steps.py HTTP/1.1\r\n");
      strcat(request,"Host: 608dev.net\r\n");
      strcat(request,"Content-Type: application/x-www-form-urlencoded\r\n");
      sprintf(request+strlen(request),"Content-Length: %d\r\n", strlen(body)); //append string formatted to end of request buffer
      strcat(request,"\r\n"); //new line from header to body
      strcat(request,body); //body
      strcat(request,"\r\n"); //header
      do_http_request("608dev.net", request, response, 1000, 6000,true);
      post_timer=millis();
    }
    
    if(is_active) {
      if(state==0) {
        if(steps!=last_steps) {
          draw();
        }
        if(type_module.update(bv)==1) {
          if(is_wifi_connected) {
            state=1;
            tft->fillRect(0,20,130,140,BACKGROUND);
            tft->setCursor(0,20,FONTSIZE);
            tft->println("Please input a year");
            tft->print(message_to_type);
          }
          else {
            tft->fillRect(0,20,130,140,BACKGROUND);
            tft->setCursor(0,20,FONTSIZE);
            tft->println("This part requires wifi connection!");
            tft->println("Long Press to go back");
            state=7;
          }
        }
      }
      else if(state==1) {
        int a=type_module.update(bv);
        if(a==1) {
          tft->fillRect(0,20,130,140,BACKGROUND);
          tft->setCursor(0,20,FONTSIZE);
          tft->println("Please input a year");
          tft->print(message_to_type);
        }
        if(a==2) {
          state=2;
          tft->fillRect(0,20,130,140,BACKGROUND);
          tft->setCursor(0,20,FONTSIZE);
          tft->print("Long press to input month");
          sprintf(request,"GET http://608dev.net/sandbox/sc/haoyangg/final_project/steps.py?user=%s&year=%s",user,message_to_type);
        }
      }
      else if(state==2) {
        if(type_module.update(bv)==1) {
          state=3;
          tft->fillRect(0,20,130,140,BACKGROUND);
          tft->setCursor(0,20,FONTSIZE);
          tft->print(message_to_type);
        }
      }
      else if(state==3) {
        int a=type_module.update(bv);
        if(a==1) {
          tft->fillRect(0,20,130,140,BACKGROUND);
          tft->setCursor(0,20,FONTSIZE);
          tft->print(message_to_type);
        }
        if(a==2) {
          state=4;
          tft->fillRect(0,20,130,140,BACKGROUND);
          tft->setCursor(0,20,FONTSIZE);
          tft->print("Long press to input day");
          sprintf(request+strlen(request),"&month=%s",message_to_type);
        }
      }
      else if(state==4) {
        if(type_module.update(bv)==1) {
          state=5;
          tft->fillRect(0,20,130,140,BACKGROUND);
          tft->setCursor(0,20,FONTSIZE);
          tft->print(message_to_type);
        }
      }
      else if(state==5) {
        int a=type_module.update(bv);
        if(a==1) {
          tft->fillRect(0,20,130,140,BACKGROUND);
          tft->setCursor(0,20,FONTSIZE);
          tft->print(message_to_type);
        }
        if(a==2) {
          state=6;
          sprintf(request+strlen(request),"&day=%s",message_to_type);
        }
      }
      else if(state==6) {
        tft->fillRect(0,20,130,140,BACKGROUND);
        tft->setCursor(0,20,FONTSIZE);
        tft->print("Please Wait");
        sprintf(request+strlen(request),"&action=check HTTP/1.1\r\n");
        strcat(request,"Host: 608dev.net\r\n");
        strcat(request,"\r\n");
        do_http_request("608dev.net", request, response, 1000, 6000,true);
        tft->fillRect(0,20,130,140,BACKGROUND);
        tft->setCursor(0,20,FONTSIZE);
        if(response[0]!='\0') {
          tft->println(response);
        }
        else {
          tft->println("Failed to connect server! Please check your wifi connection.");
        }
        tft->println("Long Press to go back");
        state=7;
      }
      else if(state==7) {
        if(bv==2) {
          state=0;
          draw();
        }
      }
    }    
    last_steps=steps;
  }
}

int Steps_Module::get_steps() {
  return steps;
}
