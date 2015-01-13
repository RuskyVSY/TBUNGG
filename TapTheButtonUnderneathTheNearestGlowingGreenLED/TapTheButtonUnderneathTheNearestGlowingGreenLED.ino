/*
Authors: Vlad Silin, Mrigank Mehta
Date: June 12, 2014

Description: This program controls a game based on the popular phone game "Piano Tiles". The point of this game
is to hit the buttons underneath the "falling" green LEDs as they reach the last position. The rate at which the LEDs
move down keeps on increasing. For every LED that is hit correctly, a point is rewarded. The game is lost if a single
LED is missed. An LED display has also been implemented in order to display the player's score and the highscore. If
the current highscore is reached or beaten, the box next to the game opens, revealing the contents. If the highscore
is not met, the box flips over.
*/


#include <Servo.h> 

const int latchPin = 2;
const int clockPin = 4;
const int dataPin = 3;
const int latchPin2 = 7;
const int clockPin2 = 8;
const int dataPin2 = 9;
const int latchPin3 = 12;
const int clockPin3 = 13;
const int dataPin3 = 11;
const int button1 = A0;
const int button2 = A1;
const int button3 = A2;
const int button4 = A3;
const int resetButton = A4;

Servo lidServo;
Servo flipServo;

unsigned long previousMillis = 0;
unsigned long currentMillis;

byte leds = 0;
byte leds2 = 0;
byte digits[] = {
  0b10000001,0b10110111,0b11000010,0b10010010,0b10110100,0b10011000,0b10001000,0b10110011,0b10000000,0b10010000,0b11111111};

int randX[4];
int increment [4];
int reading [5];
int del = 400;
int score = 1000;
int highScore = 15;

boolean hitLight1 = false;
boolean hitLight2 = false;
boolean hitLight3 = false;
boolean hitLight4 = false;
boolean right1 = false;
boolean right2 = false;
boolean right3 = false;
boolean right4 = false;

boolean scoreSub = false;
boolean resetPressed = false;
boolean disp = true;

void setup() 
{
  Serial.begin (9600);
  randomSeed(analogRead(A5));
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin2, OUTPUT);
  pinMode(dataPin2, OUTPUT);  
  pinMode(clockPin2, OUTPUT);
  pinMode(latchPin3, OUTPUT);
  pinMode(dataPin3, OUTPUT);  
  pinMode(clockPin3, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(resetButton, INPUT);
  lidServo.attach (5);
  flipServo.attach (6);
  randX[0] = random (4);
}

/*
This is the main loop of the program. It displays the current highscore until the reset button is held down.
This method runs the entire game.
*/
void loop() 
{
  if (disp)
    displayScore (highScore);
  game ();
}

/*
This is the main game method. This method randomly dispenses LEDs from the top of the screen and makes them move
down at the same (accelerating) rate relative to each other. This method also processes button presses and recognizes
losing conditions
*/
void game ()
{
  currentMillis = millis();
  reading[0] = digitalRead(button1);
  reading[1] = digitalRead(button2);
  reading[2] = digitalRead(button3);
  reading[3] = digitalRead(button4);
  reading[4] = digitalRead(resetButton);
  if (reading[4] && !resetPressed)
  {
    disp = false;
    resetPressed = true;
    resetGame();
  }
  if (resetPressed)
  {
    hitLight1 = false;
    hitLight2 = false;
    hitLight3 = false;
    hitLight4 = false;
    if (currentMillis - previousMillis > del)
    {
      previousMillis = currentMillis;
      increment[0]++;
      if (increment[0] == 8)
      {
        increment[0] = 0;
        randX[0] = random (4);
      }
      if (increment[1] >= 1)
        increment[1] ++;
      if (increment[0] == 2)
      {
        increment[1] = 1;
        randX[1] = random (4);
      }
      if (increment[2] >= 1)
        increment[2] ++;
      if (increment[0] == 4)
      {
        increment[2] = 1;
        randX[2] = random (4);
      }
      if (increment[3] >= 1)
        increment[3] ++;
      if (increment[0] == 6)
      {
        increment[3] = 1;
        randX[3] = random (4);
      }
      if (increment [0] == 7 || increment [1] == 8 || increment [2] == 8 || increment [3] == 8)
      {
        score ++;
        if (del > 200)
          del -= 2;
      }
    }
    updateShiftRegister(randX[0] , increment[0]);
    if (increment[1] >= 1)
      updateShiftRegister(randX[1] , increment[1] - 1);
    if (increment[2] >= 1)
      updateShiftRegister(randX[2] , increment[2] - 1);
    if (increment[3] >= 1)
      updateShiftRegister(randX[3] , increment[3] - 1);
    if (!hitLight1)
      right1 = false; 
    if (!hitLight2)
      right2 = false; 
    if (!hitLight3)
      right3 = false; 
    if (!hitLight4)
      right4 = false; 
    if (reading [0] && !hitLight1 || reading [1] && !hitLight2 || reading [2] && !hitLight3 || reading [3] && !hitLight4)
    {
      scoreSub = false;
      lost ();
      return;
    }
    if (hitLight1 && reading [0])
      right1 = true; 
    if (hitLight2 && reading [1])
      right2 = true; 
    if (hitLight3 && reading [2])
      right3 = true;
    if (hitLight4 && reading [3])
      right4 = true; 
    if (currentMillis - previousMillis >= del-5)
    {
      if (hitLight1 && !right1 || hitLight2 && !right2  || hitLight3 == 1 && !right3 || hitLight4 && !right4)
      {
        scoreSub = true;
        lost ();
        return;
      }
    }
  }
}

/*
This method contains the "game over" conditions. It displays different things and performs different actions depending 
on whether the player has beaten the current highscore or not.
*/
void lost ()
{
  int scoreWait;
  unsigned long currentMillis = millis ();
  if (scoreSub)
    score --;
  resetPressed = false;
  disp = true;
  while (millis ()-currentMillis <= 1500)
  {
    if (score >= highScore)
    {
      highScore = score;
      shiftRegisterSet (0, 3);
      delayMicroseconds (500);
      shiftRegisterClear (0, 3);
      shiftRegisterSet (1, 4);
      delayMicroseconds (500);
      shiftRegisterClear (1, 4);
      shiftRegisterSet (2, 3);
      delayMicroseconds (500);
      shiftRegisterClear (2, 3);
      shiftRegisterSet (3, 2);
      delayMicroseconds (500);
      shiftRegisterClear (3, 2);
    }
    else
    {
      shiftRegisterSet (0, 2);
      delayMicroseconds (500);
      shiftRegisterClear (0, 2);
      shiftRegisterSet (1, 3);
      delayMicroseconds (500);
      shiftRegisterClear (1, 3);
      shiftRegisterSet (2, 4);
      delayMicroseconds (500);
      shiftRegisterClear (2, 4);
      shiftRegisterSet (3, 5);
      delayMicroseconds (500);
      shiftRegisterClear (3, 5);
      shiftRegisterSet (3, 2);
      delayMicroseconds (500);
      shiftRegisterClear (3, 2);
      shiftRegisterSet (2, 3);
      delayMicroseconds (500);
      shiftRegisterClear (2, 3);
      shiftRegisterSet (1, 4);
      delayMicroseconds (500);
      shiftRegisterClear (1, 4);
      shiftRegisterSet (0, 5);
      delayMicroseconds (500);
      shiftRegisterClear (0, 5);
    }
  }
  if (score < highScore)
  {
    if(score > 999)
      scoreWait = 12900;
    else if(score > 99)
      scoreWait = 10600;
    else if(score > 9)
      scoreWait = 8300;
    else
      scoreWait = 6000;
    currentMillis = millis ();
    while (millis () - currentMillis <= scoreWait)
      displayScore (score);
    //rotate ();
  }
  else
  {
    //lid ();
  }
}

/*
This method resets the game to its staring state, allowing for a new game to be played.
*/
void resetGame ()
{
  leds = 0;
  leds2 = 0;
  del = 400;
  randX[0] = random (4);
  for (int x = 0; x < 4; x++)
    increment [x] = 0;
  hitLight1 = false;
  hitLight2 = false;
  hitLight3 = false;
  hitLight4 = false;
  right1 = false;
  right2 = false;
  right3 = false;
  right4 = false;
  previousMillis = currentMillis;
  score = 0;
}

/*
This method updates the shift registers, telling them which LEDs in the matrix to turn on. This method also registers
constions which must be stored in order to detect correct and incorrest button presses.
*/
void updateShiftRegister(int x, int y)
{
  shiftRegisterSet (x, y);

  if (bitRead (leds, 0) == 1 && bitRead(leds2, 7) == 1)
    hitLight1 = true;

  if (bitRead (leds, 1) == 1 && bitRead(leds2, 7) == 1)
    hitLight2 = true;

  if (bitRead (leds, 2) == 1 && bitRead(leds2, 7) == 1)
    hitLight3 = true;

  if (bitRead (leds, 3) == 1 && bitRead(leds2, 7) == 1)
    hitLight4 = true;

  shiftRegisterClear (x, y);
}

//This method pushes out data to the positions in the shift registers given in the overloads.
void shiftRegisterSet (int a, int b)
{
  bitSet(leds, a);
  bitSet(leds2, b);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, leds);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin2, LOW);
  shiftOut(dataPin2, clockPin2, MSBFIRST, leds2);
  digitalWrite(latchPin2, HIGH);
}


//This method clears data from the positions in the shift registers given in the overloads.
void shiftRegisterClear (int a, int b)
{
  bitClear(leds, a);
  bitClear(leds2, b);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, leds);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin2, LOW);
  shiftOut(dataPin2, clockPin2, MSBFIRST, leds2);
  digitalWrite(latchPin2, HIGH);  
}


//This method sets the display to show the number given in the overload.
void displaySet (int num)
{
  digitalWrite(latchPin3, LOW);
  shiftOut(dataPin3, clockPin3, MSBFIRST, digits[num]);
  digitalWrite(latchPin3, HIGH);
}


//This method is used to display the last score or the highscore, which is passed to it in the overload.
void displayScore (int s)
{
  unsigned long current = millis ();
  int ones = (s%10);
  int tens = ((s/10)%10);
  int hundreds = ((s/100)%10);
  int thousands = (s/1000);
  if (thousands != 0 && s < 1000 || s >= 1000)
  {
    current = millis ();
    while (millis () - current <= 1000)
      displaySet (thousands);
    current = millis ();
    while (millis () - current <= 150)
      displaySet (10); 
  }
  if (hundreds != 0 && s < 100 || s >= 100)
  {
    current = millis ();
    while (millis () - current <= 1000)
      displaySet (hundreds);
    current = millis ();
    while (millis () - current <= 150)
      previousMillis = current;
    displaySet (10);
  }
  if (tens != 0 && s < 10 || s >= 10)
  {
    current = millis ();
    while (millis () - current <= 1000)
      displaySet (tens);
    current = millis ();
    while (millis () - current <= 150)
      displaySet (10);
  }
  current = millis ();
  while (millis () - current <= 1000)
    displaySet (ones);
  current = millis ();
  while (millis () - current <= 2000)
    displaySet (10);
}

//This method operates the servo responsible for opening the lid of the box, in a winning condition.
void lid ()
{
  int pos;
  for(pos = 0; pos < 180; pos += 1)   
  {                                  
    lidServo.write(pos);              
    delay(15);                        
  } 
  delay (3000);
  for(pos = 180; pos>=1; pos-=1)     
  {                                
    lidServo.write(pos);               
    delay(15);                        
  } 
}

//This method operates the servo responsible for flipping over the box, in a losing condition.
void rotate ()
{
  int pos;
  for(pos = 0; pos < 180; pos += 1)   
  {                                  
    flipServo.write(pos);              
    delay(15);                        
  } 
  delay (1000);
  for(pos = 180; pos>=1; pos-=1)     
  {                                
    flipServo.write(pos);               
    delay(15);                        
  } 
}




















