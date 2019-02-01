# ArduinoBikeStopLight : Bike stop light based on Arduinos, CC1101 wireless modules and WS2812B LEDs

## Features
This project is to explore the wireless communication using cc1101 modules
At the moment the differents Arduino projects are as follows :
* The cc1101 samples ares used to exchange simple strings only
* The led_strip project is able to simulate the desired behavior using three buttons (left, right, stop)
  * An animation is displayed when either left or right button is pressed
  * The rest of the led (in the case of no animation, all of them) become fully lit when stop button is pressed
  
## Configuration
### Main project
Example for this LEDs arangment :  
``#LED:    0     1  2    3       4       5       6     7  8   9``  
``Name:[L_start][-][-][L_end][S_start][S_end][R_start][-][-][R_end]``

- LEDs
  - #define NUM_LEDS 10 -> total number of LEDs
  - #define L_start 0 -> Start of left 
  - #define L_end   3 -> End of left
  - #define S_start 4 -> Start of stop
  - #define S_end   5 -> End of stop
  - #define R_start 6 -> Start of right
  - #define R_end   9 -> End of stop
- Animations
  - #define LR_TimeOut 1000 -> time (ms) before norm color return after signalling, increase if the signal becomes red between each blink
  - #define INTV_ROLL 32    -> interval (ms) between led roll
  - #define STEP_ROLL 4     -> step (change in brightness) between led roll
- LEDs colors {R,G,B} format, between 0 (off) and 255 (full brightness)
  - const int color_led_stop[] = {200,0,0};   -> color when stop is applied
  - const int color_led_turn[] = {255,100,0}; -> color when signalling a turn (left or right)
  - const int color_led_norm[] = {40,0,0};    -> color when nothing happens
 
### cc1101 samples
- TODO

## Project structure
- README.md       -> That the file you are reading now
- Schematics      -> Fritzing schematics
- cc1101*         -> Samples used to test wireless connectivity
- led_strip_stop  -> Main arduino project

## Schematics
### Main project
* Be sure to NOT power the LEDs string with the Arduino ! They draw more than what the board can handle !
![Schema 1](https://raw.githubusercontent.com/jimmycaille/ArduinoBikeStopLight/master/Schematics/arduino_breadboard_led.png "Schema 1")

### cc1101 samples
* Be sure to check your module pinout ! Not all are made equals ;)
![Schema 2](https://raw.githubusercontent.com/jimmycaille/ArduinoBikeStopLight/master/Schematics/arduino_breadboard_cc1101.png "Schema 2")

## Log
### TODO

## Known issues
* Can't have both animations playing (left and right) in the case of an emergency

## Prerequisites (to upload Arduino code, open Visual Studio project and read Fritzing schematics)
### Arduino IDE (free) can be found here :
https://www.arduino.cc/en/main/software
### Visual Studio Community (free) can be found here :
https://visualstudio.microsoft.com
### Fritzing (free) can be found here :
http://fritzing.org/download/

## Developper infos
- Author : Jimmy Caille
- Email  : jimmy.caille.dev@gmail.com
