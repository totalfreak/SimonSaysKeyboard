#include "Arduino.h"
#include "Led.cpp"
//Button object
class Button {
public:
  Led led;
  //Button(ButtonPin, LEDPin)
  //Constructor
  //Also creating own LED object
  Button(int initPin, int ledInitPin) {
    pin = initPin;
    ledPin = ledInitPin;
    setPinMode(INPUT_PULLUP);
    led = Led(ledPin);
  }

  Button(){};

  //Returning digital value of button press
  int readValue() {
    if(digitalRead(getPin())) {
      return HIGH;
    } else
      return LOW;
  }
  //Setting pin mode
  void setPinMode(int mode) {
    pinMode(getPin(), mode);
  }

  //Activating button, and LED
  void activate() {
    activated = true;
    activateLED();
  }

  void deActivate() {
    activated = false;
    deActivateLED();
  }

  bool getActivated() {
    return activated;
  }

  void activateLED() {
    led.activate();
  }

  void deActivateLED() {
    led.deActivate();
  }

  int getPin() {
    return pin;
  }

private:
  int pin, ledPin;
  bool activated = false;
};
