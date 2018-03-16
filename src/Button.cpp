#include "Arduino.h"
#include "NewTone/NewTone.h"
#include "Led.cpp"
//Button object
class Button {
public:
  Led led;
  //Button(ButtonPin, LEDPin)
  //Constructor
  //Also creating own LED object
  Button(int initPin, int ledInitPin, int initMelody) {
    pin = initPin;
    ledPin = ledInitPin;
    melody = initMelody;
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

  //Playing my sound
  void playSound(int buzzerPin, int duration) {
    NewTone(buzzerPin, melody, duration);
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
  int pin, ledPin, melody;
  bool activated = false;
};
