#include "Arduino.h"

class Led {
public:
  Led(int initPin) {
    pin = initPin;
    pinMode(initPin, OUTPUT);
    setValue(getActivated());
  }

  Led(){};
  //Set LED state, on or off
  void setValue(int value) {
    digitalWrite(getPin(), value);
  }
  //Setting activate bool and changing state of LED.
  void activate() {
    activated = true;
    setValue(activated);
  }
  //Setting activate bool and changing state of LED.
  void deActivate() {
    activated = false;
    setValue(activated);
  }
  
  bool getActivated() {
    return activated;
  }

  int getPin() {
    return pin;
  }

private:
  int pin;
  bool activated = false;
};
