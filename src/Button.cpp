#include "Arduino.h"
#include "Led.cpp"

class Button {
public:
  Button(int initPin, int ledInitPin) {
    pin = initPin;
    ledPin = ledInitPin;
    setPinMode(INPUT_PULLUP);
    led = Led(ledPin);
  }

  Button(){};

  int readValue() {
    if(digitalRead(getPin())) {
      return HIGH;
    } else
      return LOW;
  }

  void setPinMode(int mode) {
    pinMode(getPin(), mode);
  }

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
  Led led;
  bool activated = false;
};
