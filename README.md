# TBUNGGL (Tap the Button Underneath the Nearest Glowing Green LED)

This program implements an Arduino controlled game based on the popular mobile game "Piano Tiles". 

A 4x8 array of LEDs is used as the main game display.
The point of the game is to hit the buttons underneath the "falling" green LEDs as they reach the last position. 
The rate at which the LEDs move down keeps increasing. For every LED that is hit correctly, a point is rewarded. The game is lost if a single LED is missed. 

If the current highscore is reached or beaten, a  box (perhaps containing a prize) located next to the game screen opens, revealing the contents. If the highscore is not met, the box flips over. These mechanisms are controlled by servo motors.

A seven-segment LED display has also been implemented in order to display the player's current score as well as the highscore. 

In order to overcome the output pin limitations of the Arduino microcontroller, demultiplexing and shift registers are used to allow control of the entire LED array. 

Pictures of the hardware in testing stages are included.
