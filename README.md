# YAPG - Yet Another Pong Game

*****
*The credit for this project goes mostly to my teammates [Nafee](https://github.com/AN-Akhand) & [Mobaswir](https://github.com/ezmata-101) who built the entire model or interface themselves. So big applause for them.*
*****

## Introduction
This is a simple *pong* game made with ATMega32 microcontroller. Pong is a simple *tennis-like* game that features two paddles and a ball. When one of the players misses hitting the ball with his paddle, the other gets a point. The paddle is controlled by a *Sonar Sensor* which enables the paddle to move up & down based on the distance of our hand from the Sonar. Whenever a player makes a point, the ATMega32 does the following:
- Turns the Buzzer & Vibration Motor on as heptic feedback
- Shows points of the players in LCD display
- Continues the game until a player makes 9 points
- Makes the ball move faster in next round

The buzzer is turned off automatically after 1-2 seconds.

## How to reconstruct the project
### Required Hardwares
- ATMega32 - 1 unit
- LED Matrix - 2 unit
- Ultrasonic SONAR (HC-SR04) - 2 unit
- 16x2 I2C Interface LCD Display (JHD-2X16-I2C) - 1 unit
- Buzzer - 2 unit
- Vibration Motor - 2 unit
- Decoder (74HC154) - 1 unit
- Power adapter (Output 5V, 2A) - 1 unit
- USBasp - 1 unit
- Breadboard - As Required
- Wires (Male to Male & Male to Female)- As required

### Required Softwares
- [Microchip Studio](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio)
- [Extreme burner](https://extreme-burner-avr.software.informer.com/download/)
- [Proteus](https://drive.google.com/file/d/15udSfVaX_LnWVT0cLAXcaUxck057d5bu/view?usp=sharing)
  `Note`: You can also download proteus from [here]()

### Setting up the connection
- Connect the following components to the ATMega32 as follows:
    - **Setting LED** :As you have used two `8x8 LED` matrix, it means there were total 16 rows and 16 columns. So for utilizing the pins efficiently, **short the columns** (8 columns) thinking LED in 16x8 format. Then use a `74HC154` decoder to select rows one by one. In total, you need to use 12 pins to operate two LED matrices.
    Connect the 8 columns of LEDs at `PORTB` from `PINB0` to `PINB7`. Then connect the 16 rows of LEDs with the output pins of `Decoder`. Now, input pins of `Decoder` will be connected to `PINA0` to `PINA3`.
    - **Buzzer**: Connect `VCC` to `PINC2` and `GND` to the `GND` of breadboard. For another buzzer, connect `VCC` to `PINC3` and `GND` to the `GND` of breadboard.
    - **Vibration Motor**: Connect `VCC` to `PINC2` and `GND` to the `GND` of breadboard. For another motor, connect `VCC` to `PINC3` and `GND` to the `GND` of breadboard.
    - **LCD**: Connect `VCC` to `VCC` and `GND` to the `GND` of breadboard. `SCL` will be connected to the `PINC0` and `SDA` with `PINC1`.
    - **Sonar Sensor**: `Triggers` of the two sonar sensors will be connected to `PIND0` & `PIND1`. And `Echo` will be connected to `PIND2` & `PIND3` correspondingly.

###Block Diagram
![Block Diagram]()

###Circuit Diagram
![Circuit Diagram in Proteus]()

## How to use the project
- Create a project on Atmel Studio. You cann see [this](http://atmel-studio-doc.s3-website-us-east-1.amazonaws.com/webhelp/GUID-54E8AE06-C4C4-430C-B316-1C19714D122B-en-US-1/index.html?GUID-0661DC83-89AB-44A0-8AD5-CACC9174EAE7) for reference

    `Note`: Select ***GCC C++ Executable Project*** option from the template list & ***ATMega32*** as AVR device
- Now copy the code from [main.cpp]() and paste it in your project
- Then extract this [zip]() file and load all the files in your project. For help you can follow [this](https://microchipsupport.force.com/s/article/Adding-files-from-external-directory-in-Atmel-Studio)
    `Note`: For the step: `Right click on project/folder in which you want to add the source file. click on Add-> Existing item` select the **project folder** that you have created
- Now build the solution
- Open eXtreme Burner and load the hex file. For help follow [this]()
    `Note`: Remember to select `ATMega32` as chip from the topbar.
- Connect the USBasp to ATMega32
- Now connect the following components to the ATMega32 as follows:
    - It is recommended to use an adapter to power the ATMega32 as the game interface should be turned on all the time. So connect the `pin 10 (VCC)` of the ATMega32 to the `VCC` of the adapter and the `pin 11 (GND)` of the ATMega32 to the `GND` of the adapter
    - You can short connect `pin 31 (GND)` with `pin 11 (GND)` or connect it directly with `GND` of the adapter

## Problems faced and how we solved them

- Our initial plan was to use `7 segment displays` to show the scores. But
they required a lot of pins to operate. So we used LCD display(that uses
`I2C` communication protocol) to do it instead. This lessened our number
of required pins by *12*.
- As we used ultrasonic sonar sensors, we had to use external hardware
interrupts. We decided to call our interrupt service response on both
rising and falling edges so only `INT0` and `INT1` were available to use, as
`INT2` can only support one edge response. So we were limited to two pins
for interrupts. But to implement both horizontal and vertical movements,
we had to use `4 sensors` and that would require `4 pins`. We tried to to
it with multiplexers but for some reason (might have been *propagation
delay*) it just would not work. So, later we restricted pad movements to
horizontal axis only.
- As our board’s dimension was only 16x8, it seems there were these cases
when the ball kept moving in a loop if the players don’t move their pad.
To resolve this loop issue, in code, we tried to detect a loop and give the
ball a random movement(after collision with pad).

* * * * *

**Project Supervisor**

*Dr. Md. Monirul Islam*
Professor, Department of Computer Science and Engineering,\
Bangladesh University of Engineering and Technology (BUET)
 
*Dr. A. B. M. Alim Al Islam*
Professor, Department of Computer Science and Engineering,\
Bangladesh University of Engineering and Technology (BUET)
 
*Md. Masum Mushfiq*
Lecturer, Department of Computer Science and Engineering,\
Bangladesh University of Engineering and Technology (BUET)

* * * * *

## Contributors
- [Shehabul Islam Sawraz](https://github.com/Shehabul-Islam-Sawraz)
- [Mohammad Abrar Nafee Akhand](https://github.com/AN-Akhand)
- [Mobaswirul Islam](https://github.com/ezmata-101)