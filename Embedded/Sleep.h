#ifndef Sleep_h
#define Sleep_h
#include "Arduino.h"
#include <TFT_eSPI.h>

class Sleep{
    TFT_eSPI* screen;
    uint8_t out;
    int INACTIVE_TIME;
    uint8_t choice;
    uint8_t pre_choice;
    uint8_t* grand_screen_state;

    //Color
    int BACKGROUND;
    int TEXT_COLOR;
    int SELECT_COLOR;
    int BACK_ARROW;    
    const int SELECTED_TIME = TFT_YELLOW;

  public:
    uint8_t state;
    uint8_t set_state;
  	uint32_t timer;
  	Sleep(TFT_eSPI* tft, uint8_t op, uint8_t* gss, int bg, int tc, int sc, int ba);
    void drawChoice();

  private:
    //functions
    void set_inactive_time(int sec);  
      
  public:
  	void fsm(int btn1, int btn2);
  	void set_fsm(int btn2);
};

#endif