#include "Friends.h"
#include "Requests.h"

#define BACKGROUND TFT_BLACK
#define FONTSIZE 1

Message_Module::Message_Module(TFT_eSPI* tft_to_use, MPU9255* imu_to_use, char* username):type_module(message_to_type,imu_to_use){
  state=0;
  is_active=false;
  tft=tft_to_use;
  imu=imu_to_use;
  number_of_messages=0;
  start_of_messages=0;
  user=username;
  check_message_timer=millis()-10000;
  for(int i=0;i<10;i++)
  {
    messages_buffer[i]=new char[200];
  }
}

void Message_Module::activate(){
  type_module.reset();
  is_active=true;
  tft->fillRect(0,20,130,140,BACKGROUND);
  if(number_of_messages!=0) {
    tft->setCursor(0,20,FONTSIZE);
    char to_print[100];
    sprintf(to_print,"You have %d new messages",number_of_messages);
    tft->println(to_print);
    tft->println(messages_buffer[start_of_messages]);
  } else {
    tft->setCursor(0,20,FONTSIZE);
    tft->println("You have no new message");
    tft->print("Long Press and input the username of your friend");
  }
}

void Message_Module::close() {
  state=0;
  is_active=false;
}

void Message_Module::update(int bv) {
  if(is_active) {
    if(number_of_messages!=0) {
      if(bv==1) {
        number_of_messages--;
        start_of_messages=(start_of_messages+1)%10;
        tft->fillRect(0,20,130,140,BACKGROUND);
        if(number_of_messages!=0) {
          tft->setCursor(0,20,FONTSIZE);
          char to_print[100];
          sprintf(to_print,"You have %d new message(s)",number_of_messages);
          tft->println(to_print);
          tft->println(messages_buffer[start_of_messages]);
        } else {
          tft->setCursor(0,20,FONTSIZE);
          tft->println("You have no new message");
          tft->print("Long Press and input the username of your friend");
        }
      }
    }
    else {
      if(state==0) {
        if(type_module.update(bv)==1) {
          state=1;
          tft->fillRect(0,20,130,140,BACKGROUND);
          tft->setCursor(0,20,FONTSIZE);
          tft->print(message_to_type);
        }
      }
      else if(state==1) {
        int a=type_module.update(bv);
        if(a==1) {
          tft->fillRect(0,20,130,140,BACKGROUND);
          tft->setCursor(0,20,FONTSIZE);
          tft->print(message_to_type);
        }
        if(a==2) {
          state=2;
          tft->fillRect(0,20,130,140,BACKGROUND);
          tft->setCursor(0,20,FONTSIZE);
          tft->print("Long Press and input the content");
          //Serial.println("I am going to send message to this username:");
          //Serial.println(message_to_type);
          sprintf(body,"module=message&user1=%s&user2=%s",user,message_to_type);
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
          //Serial.println("I am going to send the content:");
          //Serial.println(message_to_type);
          sprintf(body+strlen(body),"&content=%s",message_to_type);
        }
      }
      else if(state==4) {
        tft->fillRect(0,20,130,140,BACKGROUND);
        tft->setCursor(0,20,FONTSIZE);
        tft->print("Please Wait");
        sprintf(request,"POST http://608dev.net/sandbox/sc/haoyangg/final_project/friends.py HTTP/1.1\r\n");
        strcat(request,"Host: 608dev.net\r\n");
        strcat(request,"Content-Type: application/x-www-form-urlencoded\r\n");
        sprintf(request+strlen(request),"Content-Length: %d\r\n", strlen(body)); //append string formatted to end of request buffer
        strcat(request,"\r\n"); //new line from header to body
        strcat(request,body); //body
        strcat(request,"\r\n"); //header
        do_http_request("608dev.net", request, response, 1000, 6000,true);
        tft->fillRect(0,20,130,140,BACKGROUND);
        tft->setCursor(0,20,FONTSIZE);
        if(response[0]!='\0') {
        tft->println(response);
        }
        else {
          tft->println("Failed to connect server! Please check your wifi connection");
        }
        tft->println("Long Press to send a new message");
        state=0;
      }
    }
  }
  else {
    if((millis()-check_message_timer>10000||millis()<check_message_timer)&&(!do_not_request)) {
      sprintf(request,"GET http://608dev.net/sandbox/sc/haoyangg/final_project/friends.py?module=message&user=%s&space=%d HTTP/1.1\r\n",user,10-number_of_messages);
      strcat(request,"Host: 608dev.net\r\n");
      strcat(request,"\r\n");
      do_http_request("608dev.net", request, response, 1000, 6000,true);
      char* p=strtok(response,",");
      while(p!=NULL) {
        sprintf(messages_buffer[(start_of_messages+number_of_messages)%10],p);
        number_of_messages++;
        p=strtok(NULL,",");
      }
      check_message_timer=millis();
    }
  }
}

Add_Module::Add_Module(TFT_eSPI* tft_to_use, MPU9255* imu_to_use,  char* username):type_module(message_to_type,imu_to_use) {
  state=0;
  is_active=false;
  tft=tft_to_use;
  imu=imu_to_use;
  number_of_requests=0;
  start_of_requests=0;
  user=username;
  check_requests_timer=millis()-10000;
  for(int i=0;i<10;i++) {
    requests_buffer[i]=new char[50];
  }
}

void Add_Module::activate() {
  type_module.reset();
  is_active=true;
  tft->fillRect(0,20,130,140,BACKGROUND);
  if(number_of_requests!=0) {
    tft->setCursor(0,20,FONTSIZE);
    char to_print[200];
    sprintf(to_print,requests_buffer[start_of_requests]);
    sprintf(to_print+strlen(to_print)," wants to be your friend.\nShort press to accept.\nLong press to refuse");
    tft->print(to_print);
  }
  else {
    tft->setCursor(0,20,FONTSIZE);
    tft->print("Long Press to add friends");
  }
}

void Add_Module::close() {
  state=0;
  is_active=false;
}
void Add_Module::update(int bv) {
  if(is_active) {
    if(number_of_requests!=0) {
      if(bv==1) {  //On the server side, send your friend a message to let him know.
        sprintf(body,"module=add&user1=%s&user2=%s&action=accept",user,requests_buffer[start_of_requests]);
        sprintf(request,"POST http://608dev.net/sandbox/sc/haoyangg/final_project/friends.py HTTP/1.1\r\n");
        strcat(request,"Host: 608dev.net\r\n");
        strcat(request,"Content-Type: application/x-www-form-urlencoded\r\n");
        sprintf(request+strlen(request),"Content-Length: %d\r\n", strlen(body)); //append string formatted to end of request buffer
        strcat(request,"\r\n"); //new line from header to body
        strcat(request,body); //body
        strcat(request,"\r\n"); //header
        do_http_request("608dev.net", request, response, 1000, 6000,true);
        number_of_requests--;
        start_of_requests=(start_of_requests+1)%10;
        tft->fillRect(0,20,130,140,BACKGROUND);
        if(number_of_requests!=0) {
          tft->setCursor(0,20,FONTSIZE);
          char to_print[200];
          sprintf(to_print,requests_buffer[start_of_requests]);
          sprintf(to_print+strlen(to_print)," wants to be your friend.\nShort press to accept.\nLong press to refuse");
          tft->print(to_print);
        }
        else {
          tft->setCursor(0,20,FONTSIZE);
          tft->println("You have no friends requests");
          tft->print("Long Press to add friends");
        }
      }
      if(bv==2) {
        Serial.println("I will refuse this requests");//On the server side, send your friend a message to let him know.
        sprintf(body,"module=add&user1=%s&user2=%s&action=refuse",user,requests_buffer[start_of_requests]);
        sprintf(request,"POST http://608dev.net/sandbox/sc/haoyangg/final_project/friends.py HTTP/1.1\r\n");
        strcat(request,"Host: 608dev.net\r\n");
        strcat(request,"Content-Type: application/x-www-form-urlencoded\r\n");
        sprintf(request+strlen(request),"Content-Length: %d\r\n", strlen(body)); //append string formatted to end of request buffer
        strcat(request,"\r\n"); //new line from header to body
        strcat(request,body); //body
        strcat(request,"\r\n"); //header
        do_http_request("608dev.net", request, response, 1000, 6000,true);
        number_of_requests--;
        start_of_requests=(start_of_requests+1)%10;
        tft->fillRect(0,20,130,140,BACKGROUND);
        if(number_of_requests!=0) {
          tft->setCursor(0,20,FONTSIZE);
          char to_print[200];
          sprintf(to_print,requests_buffer[start_of_requests]);
          sprintf(to_print+strlen(to_print)," wants to be your friend.\nShort press to accept.\nLong press to refuse");
          tft->print(to_print);
        }
        else {
          tft->setCursor(0,20,FONTSIZE);
          tft->println("You have no friends requests");
          tft->print("Long Press to add friends");
        }
      }
    }
    else {
      if(state==0) {
        if(type_module.update(bv)==1) {
          state=1;
          tft->fillRect(0,20,130,140,BACKGROUND);
          tft->setCursor(0,20,FONTSIZE);
          tft->print(message_to_type);
        }
      }
      else if(state==1) {
        int a=type_module.update(bv);
        if(a==1) {
          tft->fillRect(0,20,130,140,BACKGROUND);
          tft->setCursor(0,20,FONTSIZE);
          tft->print(message_to_type);
        }
        if(a==2) {
          state=2;
          //Serial.println("I am going to add this friend:");
          //Serial.println(message_to_type);
        sprintf(body,"module=add&user1=%s&user2=%s&action=send",user,message_to_type);
        }
      }
      else if(state==2) {
        tft->fillRect(0,20,130,140,BACKGROUND);
        tft->setCursor(0,20,FONTSIZE);
        tft->print("Please Wait");
        sprintf(request,"POST http://608dev.net/sandbox/sc/haoyangg/final_project/friends.py HTTP/1.1\r\n");
        strcat(request,"Host: 608dev.net\r\n");
        strcat(request,"Content-Type: application/x-www-form-urlencoded\r\n");
        sprintf(request+strlen(request),"Content-Length: %d\r\n", strlen(body)); //append string formatted to end of request buffer
        strcat(request,"\r\n"); //new line from header to body
        strcat(request,body); //body
        strcat(request,"\r\n"); //header
        do_http_request("608dev.net", request, response, 1000, 6000,true);
        tft->fillRect(0,20,130,140,BACKGROUND);
        tft->setCursor(0,20,FONTSIZE);
        if(response[0]!='\0') {
          tft->println(response);
        }
        else {
          tft->println("Failed to connect server! Please check your wifi connection");
        }
        tft->println("Long Press to add another friend");
        state=0;
      }
    }
  }
  else {
    if((millis()-check_requests_timer>8000||millis()<check_requests_timer)&&(!do_not_request)) {
      //Serial.println("Add: I will check requests!");
      sprintf(request,"GET http://608dev.net/sandbox/sc/haoyangg/final_project/friends.py?module=add&user=%s&space=%d HTTP/1.1\r\n",user,10-number_of_requests);
      strcat(request,"Host: 608dev.net\r\n");
      strcat(request,"\r\n");
      do_http_request("608dev.net", request, response, 1000, 6000,true);
      char* p=strtok(response,",");
      while(p!=NULL) {
        sprintf(requests_buffer[(start_of_requests+number_of_requests)%10],p);
        number_of_requests++;
        p=strtok(NULL,",");
      }
      check_requests_timer=millis();
    }
  }
}

Nearby_Module::Nearby_Module(TFT_eSPI* tft_to_use,TinyGPSPlus* gps_to_use,char* username) {
  tft=tft_to_use;
  gps=gps_to_use;
  is_active=false;
  user=username;
  timer=millis()-9000;  
}

void Nearby_Module::activate() {
  is_active=true;
  tft->fillRect(0,20,130,140,BACKGROUND);
  tft->setCursor(0,20,FONTSIZE);
  tft->print("Short Press to Refresh");
}

void Nearby_Module::close() {
  is_active=false;
}
void Nearby_Module::update(int bv) {
  if((millis()-timer>9000||millis()<timer)&&(!do_not_request)){
    if(gps->location.isValid()) {
      //Serial.println("Nearby: I will send my lat and lon:");
      //Serial.println(gps->lat);
      //Serial.println(gps->lon);
      sprintf(body,"module=nearby&user=%s&lat=%.6lf&lon=%.6lf",user,gps->location.lat(),gps->location.lng());
      sprintf(request,"POST http://608dev.net/sandbox/sc/haoyangg/final_project/friends.py HTTP/1.1\r\n");
      strcat(request,"Host: 608dev.net\r\n");
      strcat(request,"Content-Type: application/x-www-form-urlencoded\r\n");
      sprintf(request+strlen(request),"Content-Length: %d\r\n", strlen(body)); //append string formatted to end of request buffer
      strcat(request,"\r\n"); //new line from header to body
      strcat(request,body); //body
      strcat(request,"\r\n"); //header
      do_http_request("608dev.net", request, response, 1000, 6000,true);
    }
    timer=millis();
  }
  if(is_active){
    if(bv==1){
      //Serial.println("I will refresh nearby list");
      tft->fillRect(0,20,130,140,BACKGROUND);
      tft->setCursor(0,20,FONTSIZE);
      tft->print("Please Wait");
      sprintf(request,"GET http://608dev.net/sandbox/sc/haoyangg/final_project/friends.py?module=nearby&user=%s HTTP/1.1\r\n",user);
      strcat(request,"Host: 608dev.net\r\n");
      strcat(request,"\r\n");
      do_http_request("608dev.net", request, response, 1000, 6000,true);
      tft->fillRect(0,20,130,140,BACKGROUND);
      tft->setCursor(0,20,FONTSIZE);
      if(response[0]!='\0') {
      tft->println(response);
      }
      else {
        tft->println("Failed to connect server! Please check your wifi connection");
      }
    }
  }
}

Friends_Module::Friends_Module(TFT_eSPI* tft_to_use, TinyGPSPlus* gps_to_use, MPU9255* imu_to_use, char* username):message_module(tft_to_use,imu_to_use,username),add_module(tft_to_use,imu_to_use,username),nearby_module(tft_to_use,gps_to_use,username){
  tft=tft_to_use;
  state=0;
}

void Friends_Module::activate(int a){
  state=a;
  if(a==1) {
    message_module.activate();
  }
  if(a==3) {
    nearby_module.activate();
  }
  if(a==4) {
    add_module.activate();
  }  
}

void Friends_Module::close() {
  if(state==1) {
    message_module.close();
  }
  if(state==3) {
    nearby_module.close();
  }
  if(state==4) {
    add_module.close();
  }
  last_wifi_state=true;
  state=0;
}

void Friends_Module::update(int bv, bool is_wifi_connected) {
  if(state!=0) {
    if(!is_wifi_connected&&last_wifi_state) {
      tft->fillRect(0,20,130,140,TFT_BLACK);
      tft->setCursor(0, 20, 1);
      tft->println("This app requires wifi connection!");
    }
    else if(is_wifi_connected&&!last_wifi_state) {
      activate(state);
    }
    else if(is_wifi_connected) {
      message_module.update(bv);
      nearby_module.update(bv);
      add_module.update(bv);
    }
    last_wifi_state=is_wifi_connected;
  }
  else {
    if(is_wifi_connected) {
      message_module.update(bv);
      nearby_module.update(bv);
      add_module.update(bv);
    }
  }
}
