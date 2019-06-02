#include "Sleep.h"

Sleep::Sleep(TFT_eSPI* tft, uint8_t op, uint8_t* gss, int bg, int tc, int sc, int ba){
  screen = tft;
  out = op;
  INACTIVE_TIME = 30000;
  state = 0;
  set_state = 0;
  choice = 1;
  pre_choice = 1;
  grand_screen_state = gss;

  BACKGROUND = bg;
  TEXT_COLOR = tc;
  SELECT_COLOR = sc;
  BACK_ARROW = ba;
}

void Sleep::set_inactive_time(int sec){
  INACTIVE_TIME = sec;
}

void Sleep::drawChoice() {
  screen->drawLine(0, 13, 127, 13, TEXT_COLOR);
  screen->drawString("10 sec", 5, 20, 2);
  screen->drawLine(0, 43, 127, 43, TEXT_COLOR);
  screen->drawString("30 sec", 5, 50, 2);
  screen->drawLine(0, 73, 127, 73, TEXT_COLOR);
  screen->drawString("60 sec", 5, 80, 2);
  screen->drawLine(0, 103, 127, 103, TEXT_COLOR);
  screen->drawString("Never", 5, 110, 2);
  screen->drawLine(0, 133, 127, 133, TEXT_COLOR);
}

void Sleep::fsm(int btn1, int btn2){
  switch(state){
    case 0: //on
      if((INACTIVE_TIME != -1) && (millis() - timer >= INACTIVE_TIME)){
        digitalWrite(out, LOW);
        state = 1;
      } else if(btn1 || btn2){
        timer = millis();
      }
      break;
    case 1: //off
      if(btn1 || btn2){
        digitalWrite(out, HIGH);
        state = 0;
        timer = millis();
      }
      break;
  }
}

void Sleep::set_fsm(int btn2){
  switch(set_state){
    case 0: //draw the choices
      //drawChoice();
      if(pre_choice == 0){
        screen->fillRect(105, 23, 10, 10, SELECTED_TIME);
      } else if(pre_choice == 1){
        screen->fillRect(105, 53, 10, 10, SELECTED_TIME);
      } else if(pre_choice == 2){
        screen->fillRect(105, 83, 10, 10, SELECTED_TIME);
      } else{
        screen->fillRect(105, 113, 10, 10, SELECTED_TIME);
      }
      set_state = 1;
      break;
    case 1: //allow user to select
      if (btn2 == 1) {
        choice++;
      }
      switch(choice) {
        case 0: // 10 sec
          screen->fillTriangle(6, 147, 19, 141, 19, 153, BACKGROUND);
          screen->fillRect(105, 23, 10, 10, SELECT_COLOR);
          if (btn2 == 2) {
            pre_choice = choice;
            choice = 4;
            set_state = 0;
            set_inactive_time(10000);
            screen->fillRect(105, 53, 10, 10, BACKGROUND);
            screen->fillRect(105, 83, 10, 10, BACKGROUND);
            screen->fillRect(105, 113, 10, 10, BACKGROUND);
          }
          break;
        case 1: // 30 sec
          if(pre_choice != 0){
            screen->fillRect(105, 23, 10, 10, BACKGROUND);
          } else{
            screen->fillRect(105, 23, 10, 10, SELECTED_TIME);
          }
          screen->fillRect(105, 53, 10, 10, SELECT_COLOR);
          if (btn2 == 2) {
            pre_choice = choice;
            choice = 4;
            set_state = 0;
            set_inactive_time(30000);
            screen->fillRect(105, 23, 10, 10, BACKGROUND);
            screen->fillRect(105, 83, 10, 10, BACKGROUND);
            screen->fillRect(105, 113, 10, 10, BACKGROUND);
          }
          break;
        case 2: // 60 sec
          if(pre_choice != 1){
            screen->fillRect(105, 53, 10, 10, BACKGROUND);
          } else{
            screen->fillRect(105, 53, 10, 10, SELECTED_TIME);
          }
          screen->fillRect(105, 83, 10, 10, SELECT_COLOR);
          if (btn2 == 2) {
            pre_choice = choice;
            choice = 4;
            set_state = 0;
            set_inactive_time(60000);
            screen->fillRect(105, 53, 10, 10, BACKGROUND);
            screen->fillRect(105, 23, 10, 10, BACKGROUND);
            screen->fillRect(105, 113, 10, 10, BACKGROUND);
          }
          break;
        case 3: // never
          if(pre_choice != 2){
            screen->fillRect(105, 83, 10, 10, BACKGROUND);
          } else{
            screen->fillRect(105, 83, 10, 10, SELECTED_TIME);
          }
          screen->fillRect(105, 113, 10, 10, SELECT_COLOR);
          if (btn2 == 2) {
            pre_choice = choice;
            choice = 4;
            set_state = 0;
            set_inactive_time(-1);
            screen->fillRect(105, 53, 10, 10, BACKGROUND);
            screen->fillRect(105, 83, 10, 10, BACKGROUND);
            screen->fillRect(105, 23, 10, 10, BACKGROUND);
          }
          break;
        case 4: // back arrow
          if(pre_choice != 3){
            screen->fillRect(105, 113, 10, 10, BACKGROUND);
          } else{
            screen->fillRect(105, 113, 10, 10, SELECTED_TIME);
          }
          screen->fillTriangle(6, 147, 19, 141, 19, 153, BACK_ARROW);
          break;
        case 5:
          choice = 0;
          break;
      break;
    }
  }
}
