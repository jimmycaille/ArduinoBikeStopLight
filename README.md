# ArduinoBikeStopLight : Bike stop light based on Arduinos, CC1101 wireless modules and WS2812B LEDs

## Features
This project is to explore the wireless communication using cc1101 modules
At the moment the differents Arduino projects are as follows :
* The cc1101 samples ares used to exchange simple strings only
* The led_strip project is able to simulate the desired behavior using three buttons (left, right, stop)
  * An animation is displayed when either left or right button is pressed
  * The rest of the led (in the case of no animation, all of them) become fully lit when stop button is pressed

## Project structure
- README.md       -> That the file you are reading now
- Schematics      -> Fritzing schematics
- cc1101*         -> Samples used to test wireless connectivity
- led_strip_stop  -> Main arduino project

## Schematics

![Schema 1](https://raw.githubusercontent.com/jimmycaille/ArduinoBikeStopLight/master/Schematics/arduino_breadboard_led.png "Schema 1")

### Wiring
* Be sure to NOT power the LEDs string with the Arduino ! They draw more than what the board can handle !

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
