#include "Arduino.h"
#include "NewTone/NewTone.h"
#include <EEPROM.h>
#include "Button.cpp"


struct HighScore {
  byte score;
  char name[3];
};

//How many buttons are in game
const int amountOfButtons = 4;

//LED sequence size
const int sequenceSize = 100;

//Level
int level;
int startLevel = 2;

//Highscore
HighScore highscores[sequenceSize];

//EEPROM variables
int addr = 0;
byte score;

/*
0 = Game initializing;
1 = Playing sequence;
2 = Accepting input;
3 = Entering highscore name;
*/
int gameState = 0;

//Game loss LED pin;
int gameLossPin = 2;
int gameLossFrequency = 200;

//Buzzer pin
int buzzerPin = 11;

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
Button buttons[amountOfButtons] = {Button(10, 6, 400), Button(9, 5, 500), Button(8, 4, 600), Button(7, 3, 700)};
//Button buttons = {button1, button2, button3, button4, button5, button6, button7, button8, button9, button10, button11, button12};

void setup() {

  scrollThroughColours(100, 5, false);
  //Serial.begin(9600);
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
          buttons[i].playSound(buzzerPin, timeDelta);
          delay(50);
          buttons[i].deActivate();
          playedSequence[playedCounter] = i;
          //Serial.println("\nPressed button: " + String(i));
          pressButton(buttons[i]);
        }
      }
    }
  }
}

void startUpGame() {
  //Initialize game loss pin
  pinMode(gameLossPin, OUTPUT);
  //Generate the led sequence
  genSequence();
  //Reset level to start level
  level = startLevel;
  ////Serial print new game message
  //Serial.println("\nStarting new game");
  //Waiting a while
  delay(timeDelta*3);
  //Play the sequence
  playSequence();
}

void checkSequence() {
  for(int i = 0; i < level; i++) {
    if(playedSequence[i] != ledSequence[i]) {
      wrongButton();
      return;
    }
  }
  rightButton();
}

void pressButton(Button button) {
  checkSequence();

}

//If the right button pressed
void rightButton() {
  //If the sequence was played through, go up on level
  if(playedCounter >= level-1) {
    delay(timeDelta);
    upOneLevel();
  } else {
    //Haven't played through the sequence
    playedCounter++;
  }
}

//Reset game
void wrongButton() {
  loseGame(75, 5);
  startUpGame();
}
//Pretty startup colours
void scrollThroughColours(int speed, int rounds, bool pingpong) {
  int round = 0;
  while(round < rounds) {
    if(!pingpong) {
      for(int i = 0; i < amountOfButtons; i++) {
        buttons[i].led.activate();
        buttons[i].playSound(buzzerPin, timeDelta);
        delay(speed);
        buttons[i].led.deActivate();
      }
    } else {
      for(int i = 0; i < amountOfButtons; i++) {
        buttons[i].led.activate();
        buttons[i].playSound(buzzerPin, timeDelta);
        delay(speed);
        buttons[i].led.deActivate();
      }
      for(int i = amountOfButtons; i > 0; i--) {
        buttons[i].led.activate();
        buttons[i].playSound(buzzerPin, timeDelta);
        delay(speed);
        buttons[i].led.deActivate();
      }
    }
    round++;
  }
}

//Blink with loser LED
void loseGame(int speed, int blinks) {
  for(int i = 0; i < blinks; i++) {
    digitalWrite(gameLossPin, HIGH);
    NewTone(buzzerPin, gameLossFrequency, timeDelta/4);
    delay(speed);
    digitalWrite(gameLossPin, LOW);
    NewTone(buzzerPin, gameLossFrequency, timeDelta/4);
    delay(speed);
  }
}
//Read and save highscores in highscores array
void readHighscores() {
  int romSize = (int) EEPROM.length();
  for(int i = 0; i < romSize; i += sizeof(byte)) {
    EEPROM.get(i, highscores[i]);
  }
}

void evaluateScore() {
  readHighscores();
  for(int i = 0; i < sequenceSize; i++) {
    //Make loop that goes through highscores
  }
}

//Reset playedSequence and increase level by one
void upOneLevel() {
  for(int i = 0; i < sequenceSize; i++) {
    playedSequence[i] = ledSequence[i];
  }
  playedCounter = 0;
  level++;
  playSequence();
}

//Play the sequence at the current level
void playSequence() {
  //Serial.println("ledSequence");
  for(int i = 0; i < level; i++) {
    //Serial.print(String(ledSequence[i]) + " ");
  }
  gameState = 1;
  for(int i = 0; i < level; i++) {
    buttons[ledSequence[i]].led.activate();
    buttons[ledSequence[i]].playSound(buzzerPin, timeDelta);
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
