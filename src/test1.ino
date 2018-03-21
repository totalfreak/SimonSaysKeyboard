#include "Arduino.h"
#include "NewTone/NewTone.h"
#include <EEPROM.h>
#include "Button.cpp"
#include <LiquidCrystal.h>


struct HighScore {
  byte score;
  char name[4];
};

//How many buttons are in game
const int amountOfButtons = 4;

//LED sequence size
const int sequenceSize = 100;

//Level
int level;
int startLevel = 2;
int levelWhenWon = 10000;
bool firstButtonClick = false;

//Highscore
HighScore highscores[10];

//EEPROM variables
int addr = 0;
byte score;

int amountOfHighscores = 10;
char enteredName[4] = {'A', 'A', 'A', '\0'};
char alphabet[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
int alphabetSize = 25;
int alphabetSelector1 = 0, alphabetSelector2 = 0, alphabetSelector3 = 0;


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

//LCD STUFF

LiquidCrystal lcd(31, 33, 35, 37, 39, 41);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  for(int i = 0; i < amountOfHighscores; i++) {
    highscores[i].score = 2;
    highscores[i].name[0] = 'a';
    highscores[i].name[1] = 'a';
    highscores[i].name[2] = 'a';
    highscores[i].name[3] = '\0';
  }
  Serial.println("sizeof(highscores[0]): " + String(sizeof(highscores[0])));
  //Reset all highscores to aaa and 2 score.
  //writeHighscores();
  scrollThroughColours(100, 5, false);
  //Serial.begin(9600);
  //Seeding random function
  randomSeed(analogRead(0));

  //Starting LCD
  lcd.begin(16, 2);
  lcd.setCursor(3, 0);
  lcd.print("Welcome to");
  delay(timeDelta*2);
  lcd.setCursor(3, 1);
  lcd.print("Simon Says");
  lcd.setCursor(0, 0);
  delay(timeDelta*5);
  //Starting game, generating sequence and such
  startUpGame();

  //Clearing the EEPROM
  /*for (int i = 0 ; i < EEPROM.length() ; i++) {
   EEPROM.write(i, 0);
 }*/
}

void loop() {
  if(gameState == 1) {

  };
  //If in input state
  if(gameState == 2) {
    //Going over every button and activating LED when it is pressed
    for(int i = 0; i < amountOfButtons; i++) {
      if(!buttons[i].readValue()) {
        buttons[i].activate();
      } else {
        if(buttons[i].getActivated()) {
          if(!firstButtonClick) {
            firstButtonClick = true;
            displayLevel();
          }
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

  if(gameState == 3) {

  };
}

void startUpGame() {
  firstButtonClick = false;
  //Initialize game loss pin
  pinMode(gameLossPin, OUTPUT);
  //Generate the led sequence
  genSequence();
  //Reset level to start level
  level = startLevel;
  //Saving highscores in highscores array
  orderHighscores();
  //readHighscores();
  for(int i = 0; i < amountOfHighscores; i++) {
    /*String(highscores[i].name[0]) + String(highscores[i].name[1]) + String(highscores[i].name[2])*/
    Serial.println("highscore[" + String(i) + "].name: " + String(highscores[i].name));
    Serial.println("highscore[" + String(i) + "].score: " + String(highscores[i].score) + "\n");
  }
  ////Serial print new game message
  //Serial.println("\nStarting new game");
  //Waiting a while
  delay(timeDelta*3);

  //Displaying the highest score
  displayHighestScore();

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
  levelWhenWon = level;
  evaluateScore();
  gameState = 1;
  startUpGame();
}

//Read and save highscores in highscores array
void readHighscores() {
  for(int i = 0; i < amountOfHighscores; i++) {
    EEPROM.get(i*sizeof(highscores[0]), highscores[i]);
  }
}

void evaluateScore() {
  gameState = 3;
  orderHighscores();
  for(int i = 0; i < amountOfHighscores; i++) {
    if(level > highscores[i].score) {
      Serial.println("Your score: " + String(levelWhenWon));
      Serial.println("Replaced score: " + String(highscores[i].score + " at position " + String(i-1)));
      enterName();
      highscores[i-1].name[0] = enteredName[0];
      highscores[i-1].name[1] = enteredName[1];
      highscores[i-1].name[2] = enteredName[2];
      highscores[i-1].name[3] = enteredName[3];
      //strcpy(highscores[i].name, enteredName);
      highscores[i-1].score = levelWhenWon;
      Serial.println("Winner, winner, chicken dinner: " + String(highscores[i].name) + " " + String(highscores[i].score));
      for(int j = i; j < amountOfHighscores; j++) {
        highscores[j+2] = highscores[j+1];
      }
      i = amountOfHighscores;
      writeHighscores();
      displayHighestScore();
      break;
    }
  }
}

void enterName() {
displayEnterName();
bool takeName = false;
Serial.println("Enter a name: ");
while(!takeName) {
  if(!buttons[amountOfButtons-1].readValue()) {
    buttons[amountOfButtons-1].activate();
  } else {
    if(buttons[amountOfButtons-1].getActivated()) {
      takeName = true;
      buttons[amountOfButtons-1].deActivate();
      //Hardcoded name here, make enter name LCD here.

    }
  }
  for(int i = 0; i < amountOfButtons; i++) {
    if(!buttons[i].readValue()) {
      buttons[i].activate();
    } else {
      if(buttons[i].getActivated()) {
        if (i == 0) {
          alphabetSelector1++;
          if(alphabetSelector1 > alphabetSize) {
            alphabetSelector1 = 0;
          }
        } else
        if (i == 1) {
          alphabetSelector2++;
          if(alphabetSelector2 > alphabetSize) {
            alphabetSelector2 = 0;
          }
        } else
        if (i == 2) {
          alphabetSelector3++;
          if(alphabetSelector3 > alphabetSize) {
            alphabetSelector3 = 0;
          }
        }

        buttons[i].deActivate();
        enteredName[0] = alphabet[alphabetSelector1];
        enteredName[1] = alphabet[alphabetSelector2];
        enteredName[2] = alphabet[alphabetSelector3];
        enteredName[3] = '\0';
        if(i == amountOfButtons-1) {
          takeName = true;
          alphabetSelector1 = 0;
          alphabetSelector2 = 0;
          alphabetSelector3 = 0;
        }
        displayEnterName();
      }
    }
  }
  }
}

void displayEnterName() {
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Enter you name");
  lcd.setCursor(0, 1);
  lcd.print(String(enteredName));
}

void displayLevel() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("LEVEL");
  lcd.setCursor(6, 1);
  lcd.print(String(level));
}

void displayHighestScore() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Highscore " + String(highscores[0].score));
  lcd.setCursor(0, 1);
  lcd.print("By " + String(highscores[0].name));
}


void orderHighscores() {
  readHighscores();
  for(int j = 0; j < amountOfHighscores; j++) {
    for(int i = 0; i < amountOfHighscores; i++) {
      if(highscores[i].score < highscores[i+1].score) {
        HighScore tempVal = highscores[i+1];
        highscores[i+1] = highscores[i];
        highscores[i] = tempVal;
      }
    }
  }
}

void writeHighscores() {
  for(int i = 0; i < amountOfHighscores; i++) {
    EEPROM.put(i*sizeof(highscores[0]), highscores[i]);
  }
}

//Reset playedSequence and increase level by one
void upOneLevel() {
  for(int i = 0; i < sequenceSize; i++) {
    playedSequence[i] = ledSequence[i];
  }
  playedCounter = 0;
  level++;
  displayLevel();
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
