/*
Authors: Vlad Silin, Mrigank Mehta
Date: June 12, 2014

Description: This program controls a game based on the popular mobile game "Piano Tiles". The point of this game
is to hit the buttons underneath the "falling" green LEDs as they reach the last position. The rate at which the LEDs
move down keeps increasing. For every LED that is hit correctly, a point is rewarded. The game is lost if a single
LED is missed. An LED display has also been implemented in order to display the player's score and the highscore. If
the current highscore is reached or beaten, the box next to the game opens, revealing the contents. If the highscore
is not met, the box flips over.
*/


#include <Servo.h> 

// Arduino pin constants
const int LATCH_PIN = 2;
const int CLOCK_PIN = 4;
const int DATA_PIN = 3;
const int BOX_LID_SERVO_PIN = 5;
const int BOX_FLIP_SERVO_PIN = 6;
const int LATCH_PIN2 = 7;
const int CLOCK_PIN2 = 8;
const int DATA_PIN2 = 9;
const int LATCH_PIN3 = 12;
const int CLOCK_PIN3 = 13;
const int DATA_PIN3 = 11;
const int BUTTON1_PIN = A0;
const int BUTTON2_PIN = A1;
const int BUTTON3_PIN = A2;
const int BUTTON4_PIN = A3;
const int RESET_BUTTON_PIN = A4;

// Gameplay setting constants
const int MOVEMENT_DELAY_CAP = 200;
const int MOVEMENT_DELAY_DECREMENT = 2;
const int REACTION_DELAY_FORGIVENESS = 5;

// Variables for timing operation convenience
unsigned long previousMillis = 0;
unsigned long currentMillis;

// Servo objects for servo motor control
Servo boxLidServo;
Servo boxFlipServo;

// The bytes representing LED banks controlled by 2 shift registers
byte ledBank1 = 0;
byte ledBank2 = 0;

// The bit patterns which must be pushed out to the seven-segment display to
// show numbers
byte sevenSegmentDisplayDigits[] = {
    0b10000001,
    0b10110111,
    0b11000010,
    0b10010010,
    0b10110100,
    0b10011000,
    0b10001000,
    0b10110011,
    0b10000000,
    0b10010000,
    0b11111111
};

// Variables required for main game logic
int randomLEDSelection[4];
int lightIncrement[4];
int isGameButtonPressed[4];
int isResetButtonPressed;
boolean column1LightHasReachedBottom = false;
boolean column2LightHasReachedBottom = false;
boolean column3LightHasReachedBottom = false;
boolean column4LightHasReachedBottom = false;
boolean userHitColumn1LightCorrectly = false;
boolean userHitColumn2LightCorrectly = false;
boolean userHitColumn3LightCorrectly = false;
boolean userHitColumn4LightCorrectly = false;
boolean shouldSubtractFromScore = false;

// Game status variables
int lightMovementDelay = 400;
int score = 1000;
int highScore = 15;

boolean hasResetBeenPressed = false;
boolean shouldDisplayHighscore = true;

void setup() {
    Serial.begin(9600);

    randomSeed(analogRead(A5));

    pinMode(LATCH_PIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);  
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(LATCH_PIN2, OUTPUT);
    pinMode(DATA_PIN2, OUTPUT);  
    pinMode(CLOCK_PIN2, OUTPUT);
    pinMode(LATCH_PIN3, OUTPUT);
    pinMode(DATA_PIN3, OUTPUT);  
    pinMode(CLOCK_PIN3, OUTPUT);

    pinMode(BUTTON1_PIN, INPUT);
    pinMode(BUTTON2_PIN, INPUT);
    pinMode(BUTTON3_PIN, INPUT);
    pinMode(BUTTON4_PIN, INPUT);
    pinMode(RESET_BUTTON_PIN, INPUT);

    boxLidServo.attach(BOX_LID_SERVO_PIN);
    boxFlipServo.attach(BOX_FLIP_SERVO_PIN);

    randomLEDSelection[0] = random (4);
}

/*
This is the main loop of the program. It displays the current highscore until the reset button is held down.
This method runs the playGame method continuously to animate the game and accept user input.
*/
void loop() {
    if (shouldDisplayHighscore) {
        displayScore(highScore);
        playGame();
    }
}

/*
This is the main game method. This method randomly dispenses LEDs from the top of the screen and makes them move
down at the same (accelerating) rate relative to each other. This method also processes button presses and recognizes
losing conditions. The logic is complex because the animation must be done in a scanning pattern due to the nature
of the demultimplexing techniques used.
*/
void playGame() {
    currentMillis = millis();

    checkIfControlButtonsPressed();
    checkIfResetButtonPressed();

    if (hasResetBeenPressed) return;

    resetFinalLightPositionMarkers();

    setUpLightPositionData();
    updateShiftRegistersWithLightPositions();

    setUserInputStateWhenLightsNotOnBottom();

    // Loss condition when button pressed before light reaches bottom
    if (isGameButtonPressed[0] && !column1LightHasReachedBottom || 
        isGameButtonPressed[1] && !column2LightHasReachedBottom || 
        isGameButtonPressed[2] && !column3LightHasReachedBottom ||
        isGameButtonPressed[3] && !column4LightHasReachedBottom) {

        shouldSubtractFromScore = false;
        gameOver();
        return;
    }

    setCorrectUserInputState();

    // Loss condition when button pressed after light has reached bottom, but
    // with too long of a delay
    if (currentMillis - previousMillis >= 
        lightMovementDelay - REACTION_DELAY_FORGIVENESS) {
        if (column1LightHasReachedBottom && !userHitColumn1LightCorrectly || 
            column2LightHasReachedBottom && !userHitColumn2LightCorrectly || 
            column3LightHasReachedBottom && !userHitColumn3LightCorrectly || 
            column4LightHasReachedBottom && !userHitColumn4LightCorrectly) {

            // Should subtract the point that was given upon light reaching bottom
            shouldSubtractFromScore = true;
            gameOver();
            return;
        }
    }
}

void checkIfControlButtonsPressed() {
    isGameButtonPressed[0] = digitalRead(BUTTON1_PIN);
    isGameButtonPressed[1] = digitalRead(BUTTON2_PIN);
    isGameButtonPressed[2] = digitalRead(BUTTON3_PIN);
    isGameButtonPressed[3] = digitalRead(BUTTON4_PIN);
} 

void checkIfResetButtonPressed() {
    isResetButtonPressed = digitalRead(RESET_BUTTON_PIN);
    if (isResetButtonPressed && !hasResetBeenPressed) {
        shouldDisplayScore = false;
        hasResetBeenPressed = true;
        resetGame();
    }
}

void resetFinalLightPositionMarkers() {
    column1LightHasReachedBottom = false;
    column2LightHasReachedBottom = false;
    column3LightHasReachedBottom = false;
    column4LightHasReachedBottom = false;
}

void setUpLightPositionData() {
    if (currentMillis - previousMillis > lightMovementDelay) {
        previousMillis = currentMillis;
        lightIncrement[0]++;
        if (lightIncrement[0] == 8) {
            lightIncrement[0] = 0;
            randomLEDSelection[0] = random(4);
        }
        if (lightIncrement[1] >= 1) {
            lightIncrement[1] ++;
        }
        if (lightIncrement[0] == 2) {
            lightIncrement[1] = 1;
            randomLEDSelection[1] = random(4);
        }
        if (lightIncrement[2] >= 1) {
            lightIncrement[2]++;
        }   
        if (lightIncrement[0] == 4) {
            lightIncrement[2] = 1;
            randomLEDSelection[2] = random(4);
        }
        if (lightIncrement[3] >= 1) {
            lightIncrement[3]++;
        }
        if (lightIncrement[0] == 6) {
            lightIncrement[3] = 1;
            randomLEDSelection[3] = random(4);
        }

        // If a light reaches the final position, increment the score.
        // Keep speeding up the movement speed until the set cap.
        if (lightIncrement [0] == 8 || lightIncrement [1] == 8 || 
            lightIncrement [2] == 8 || lightIncrement [3] == 8) {
            score++;
            if (lightMovementDelay > MOVEMENT_DELAY_CAP) {
                lightMovementDelay -= MOVEMENT_DELAY_DECREMENT;
            }      
        }
    }
}

void updateShiftRegistersWithLightPositions() {
    updateShiftRegister(randomLEDSelection[0] , lightIncrement[0]);

    if (lightIncrement[1] >= 1) {
        updateShiftRegister(randomLEDSelection[1], lightIncrement[1] - 1);
    }

    if (lightIncrement[2] >= 1) {
        updateShiftRegister(randomLEDSelection[2], lightIncrement[2] - 1);
    }

    if (lightIncrement[3] >= 1) {
        updateShiftRegister(randomLEDSelection[3], lightIncrement[3] - 1);
    }
}

void setUserInputStateWhenLightsNotOnBottom() {
    if (!column1LightHasReachedBottom) {
        userHitColumn1LightCorrectly = false;
    }
    if (!column2LightHasReachedBottom) {
        userHitColumn2LightCorrectly = false;
    }
    if (!column3LightHasReachedBottom) {
        userHitColumn3LightCorrectly = false;
    }
    if (!column4LightHasReachedBottom) {
        userHitColumn4LightCorrectly = false;
    }
}

void setCorrectUserInputState() {
    if (column1LightHasReachedBottom && isGameButtonPressed[0]) {
        userHitColumn1LightCorrectly = true;
    }
    if (column2LightHasReachedBottom && isGameButtonPressed[1]) {
        userHitColumn2LightCorrectly = true;
    }
    if (column3LightHasReachedBottom && isGameButtonPressed[2]) {
        userHitColumn3LightCorrectly = true;
    }
    if (column4LightHasReachedBottom && isGameButtonPressed[3]) {
        userHitColumn4LightCorrectly = true;
    }
}

/*
This method contains the "game over" conditions. It displays different things and performs different actions depending 
on whether the player has beaten the current highscore or not.
*/
void gameOver() {
    int scoreWait;

    if (shouldSubtractFromScore) {
        score --;
    }

    hasResetBeenPressed = false;
    shouldDisplayScore = true;
    while (millis() - currentMillis <= 1500) {
        if (score >= highScore) {
            displayVictoryPicture();
        } else {
            displayLossPicture();
        }
    }

    // Determine how long to display the current score digits for after a loss.
    // If the user beat the highscore, the prize box is opened and the highscore
    // is displayed right away (this is done from the main loop)
    if (score < highScore) {
        if (score > 999) {
            scoreWait = 12900;
        } else if (score > 99) {
            scoreWait = 10600;
        } else if (score > 9) {
            scoreWait = 8300;
        } else {
            scoreWait = 6000;
        }
        currentMillis = millis ();

        while (millis() - currentMillis <= scoreWait) {
            displayScore(score);
        }

        rotatePrizeBox();
    } else {
        openPrizeBoxLid();
    }
}

void displayVictoryPicture() {
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

void displayLossPicture() {
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

/*
This method resets the game to its staring state, allowing for a new game to be played.
*/
void resetGame() {
    ledBank1 = 0;
    ledBank2 = 0;

    lightMovementDelay = 400;
    randomLEDSelection[0] = random (4);

    for (int x = 0; x < 4; x++) {
        increment[x] = 0;
    }

    column1LightHasReachedBottom = false;
    column2LightHasReachedBottom = false;
    column3LightHasReachedBottom = false;
    column4LightHasReachedBottom = false;
    userHitColumn1LightCorrectly = false;
    userHitColumn2LightCorrectly = false;
    userHitColumn3LightCorrectly = false;
    userHitColumn4LightCorrectly = false;

    previousMillis = currentMillis;

    score = 0;
}

/*
This method updates the shift registers, telling them which LEDs in the matrix to turn on. This method also registers
conditions which must be stored in order to detect correct and incorrect button presses.
*/
void updateShiftRegister(int bank1Setting, int bank2Setting) {
    shiftRegisterSet(bank1Setting, bank2Setting);

    if (bitRead(ledBank1, 0) == 1 && bitRead(ledBank2, 7) == 1) {
        column1LightHasReachedBottom = true;
    }

    if (bitRead(ledBank1, 1) == 1 && bitRead(ledBank2, 7) == 1) {
        column2LightHasReachedBottom = true;
    }

    if (bitRead(ledBank1, 2) == 1 && bitRead(ledBank2, 7) == 1) {
        column3LightHasReachedBottom = true;
    }

    if (bitRead(ledBank1, 3) == 1 && bitRead(ledBank2, 7) == 1) {
        column4LightHasReachedBottom = true;
    }

    shiftRegisterClear(bank1Setting, bank2Setting);
}

// This method pushes out data to the positions in the shift registers given in the overloads.
void shiftRegisterSet(int bank1Setting, int bank2Setting) {
    bitSet(ledBank1, bank1Setting);
    bitSet(ledBank2, bank2Setting);
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, ledBank1);
    digitalWrite(LATCH_PIN, HIGH);
    digitalWrite(LATCH_PIN2, LOW);
    shiftOut(DATA_PIN2, CLOCK_PIN2, MSBFIRST, ledBank2);
    digitalWrite(LATCH_PIN2, HIGH);
}


// This method clears data from the positions in the shift registers given in the overloads.
void shiftRegisterClear(int bank1Setting, int bank2Setting) {
    bitClear(ledBank1, bank1Setting);
    bitClear(ledBank2, bank2Setting);
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, leds);
    digitalWrite(LATCH_PIN, HIGH);
    digitalWrite(LATCH_PIN2, LOW);
    shiftOut(DATA_PIN2, CLOCK_PIN2, MSBFIRST, leds2);
    digitalWrite(LATCH_PIN2, HIGH);  
}


// This method sets the display to show the number given in the overload.
void displaySet(int num) {
    digitalWrite(LATCH_PIN3, LOW);
    shiftOut(DATA_PIN3, CLOCK_PIN3, MSBFIRST, sevenSegmentDisplayDigits[num]);
    digitalWrite(LATCH_PIN3, HIGH);
}


// This method is used to display the last score or the highscore, which is passed to it in the overload.
// Since only a single digit display could be obtained, the digits must be displayed sequentially.
void displayScore(int score) {
    int ones = score % 10;
    int tens = (score / 10) % 10;
    int hundreds = (score / 100) % 10;
    int thousands = score / 1000;

    displayThousands(thousands);
    displayHundreds(hundreds);
    displayTens(tens);
    displayOnes(ones);

    int currentTime = millis ();
    while (millis () - currentTime <= 2000) {
        clearSevenSegmentDisplay();
    }
}

void displayThousands(int thousands) {
    if (thousands != 0 && s < 1000 || s >= 1000) {
        int currentTime = millis ();
        while (millis () - currentTime <= 1000) {
            displaySet(thousands);
        }

        currentTime = millis ();
        while (millis () - currentTime <= 150) {
            clearSevenSegmentDisplay(); 
        }
    }
}

void displayHundreds(int hundreds) {
    if (hundreds != 0 && s < 100 || s >= 100) {
        int currentTime = millis ();
        while (millis () - current <= 1000) {
            displaySet (hundreds);
        }

        currentTime = millis ();
        while (millis () - currentTime <= 150) {
            clearSevenSegmentDisplay();
        }
    }
}

void displayTens(int tens) {
    if (tens != 0 && s < 10 || s >= 10) {
        int currentTime = millis ();
        while (millis () - currentTime <= 1000) {
            displaySet (tens);
        }

        currentTime = millis ();

        while (millis () - currentTime <= 150) {
            clearSevenSegmentDisplay();
        }
    }
}

void displayOnes() {
    int currentTime = millis ();
    while (millis () - currentTime <= 1000) {
        displaySet (ones);
    }
}

void clearSevenSegmentDisplay() {
    displaySet(10);
}

// This method operates the servo responsible for opening the lid of the box, in a winning condition.
void openPrizeBoxLid() {
    int position;

    for (position = 0; position < 180; position += 1) {                                  
        boxLidServo.write(position);              
        delay(15);                        
    }
 
    delay (3000);

    for (position = 180; position >= 1; position -= 1) {                                
        boxLidServo.write(position);               
        delay(15);                        
    } 
}

// This method operates the servo responsible for flipping over the box, in a losing condition.
void rotatePrizeBox() {
    int position;

    for (position = 0; position < 180; position += 1) {                                  
        boxFlipServo.write(position);              
        delay(15);                        
    } 

    delay (1000);

    for (position = 180; position >= 1; position -= 1) {                                
        boxFlipServo.write(position);               
        delay(15);                        
    } 
}

