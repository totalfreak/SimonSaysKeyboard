#include "Arduino.h"
#include "Button.cpp"

//How many buttons are in game
const int amountOfButtons = 2;

//LED sequence size
const int sequenceSize = 100;

//Level
int level;
int startLevel = 2;

/*
0 = Game initializing;
1 = Playing sequence;
2 = Accepting input
*/
int gameState = 0;

//Time between light change
int timeDelta = 300;

//Entire LED sequence
int ledSequence[sequenceSize];

//The replica sequence that will be modified when buttons are pressed
int playedSequence[sequenceSize];
//The counter going through the ledSequence when a right button is pressed
int playedCounter = 0;

//Array containing the buttons
//                            Button(ButtonPin, LEDPin)
Button buttons[amountOfButtons] = {Button(12, 11), Button(13, 10)};
//Button buttons = {button1, button2, button3, button4, button5, button6, button7, button8, button9, button10, button11, button12};

void setup() {
  //Seeding random function
  randomSeed(analogRead(0));
  //Starting game, generating sequence and such
  startUpGame();
}

void loop() {
  //If in input state
  if(gameState == 2) {
    //Going over every button and activating led when it is pressed
    for(int i = 0; i < amountOfButtons; i++) {
      if(!buttons[i].readValue()) {
        buttons[i].activate();
      } else {
        if(buttons[i].getActivated()) {
          delay(50);
          playedSequence[playedCounter] = i;
          checkSequence();
          buttons[i].deActivate();
        }
      }
    }
  }
}

void startUpGame() {
  //Generate the led sequence
  genSequence();
  level = startLevel;
  delay(timeDelta*3);
  //Play the sequence
  playSequence();
}

void checkSequence() {
  if(playedSequence != ledSequence) {
    wrongButton();
  } else {
    rightButton();
  }
}

//If the right button pressed
void rightButton() {
  //If the sequence was played through, go up on level
  if(playedCounter >= level) {
    upOneLevel();
  } else {
    //Haven't played through the sequence
    playedCounter++;
  }
}

//Reset game
void wrongButton() {
  startUpGame();
}

//Reset playedSequence and increase level by one
void upOneLevel() {
  for(int i = 0; i < sequenceSize; i++) {
    playedSequence[i] = ledSequence[i];
  }
  level++;
  playSequence();
}

//Play the sequence at the current level
void playSequence() {
  gameState = 1;
  for(int i = 0; i < level; i++) {
    buttons[ledSequence[i]].led.activate();
    delay(timeDelta);
    buttons[ledSequence[i]].led.deActivate();
    delay(timeDelta);
  }
  gameState = 2;
}
//Generating the led sequence
void genSequence() {
  for(int i = 0; i < sequenceSize; i++) {
    int rando = random(0, amountOfButtons);
    ledSequence[i] = rando;
    playedSequence[i] = rando;
  }
}
