#include "Arduino.h"
#include "Button.cpp"

const int amountOfButtons = 2;

Button buttons[amountOfButtons] = {Button(12, 11), Button(13, 10)};
//Button buttons = {button1, button2, button3, button4, button5, button6, button7, button8, button9, button10, button11, button12};

void setup() {

}

void loop() {
  for(int i = 0; i < amountOfButtons; i++) {
    if(!buttons[i].readValue()) {
      buttons[i].activate();
    } else {
      buttons[i].deActivate();
    }
  }
}
