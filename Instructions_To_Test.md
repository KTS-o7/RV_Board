# Instructions to run the Improved Testing suite

Hello there !
RV all in one IoT board is powered by LPC 2148 Microcontroller and supports wide array of peripherals. This document will help you to test the peripherals on the board.

## General instructions :

- The board is powered by a 5V DC supply. The supply can be provided by a USB cable or a 5V DC adapter.
- Connect the board to your computer using a USB cable.
- Then launch the Keil IDE, build the hex file and flash it to the board. ( Using flash magic or something similar )
- The board will be detected as a COM port on your computer.

## 1. Testing the LEDs

- Select `1` in the Matrix keypad to test the LEDs.
- This will start the Counter Program
- `SW2` will start a decimal up counter on the LEDs.
- `SW3` will start a decimal down counter on the LEDs.
- `SW4` will start a Ring counter (Left shift ) on the LEDs.
- `SW5` will start a Ring counter (Right shift ) on the LEDs.
- `SW6` will exit the Counter test.

## 2. Testing the 7 Segment Display

- Select `2` in the Matrix keypad to test the 7 Segment Display.
- This will start the 7 Segment Display Program
- It will alternatively glow all the segments.

## 3. Testing the Stepper Motor

- Select `3` in the Matrix keypad to test the Stepper Motor.
- Connect the stepper motor in the appropriate pins.
- This will start the Stepper Motor Program
- Stepper motor will rotate in clockwise direction and then in anti-clockwise direction.

## 4. Testing the DAC

- Select `4` in the Matrix keypad to test the DAC.
- This will start the DAC Program
- Connect the DAC output to an oscilloscope to see the output. ( Remove the Jumper pins )
- `SW2` will generate a Sine wave.
- `SW3` will generate a Square wave.
- `SW4` will generate a triangular wave.
- `SW5` will generate a sawtooth wave.
- `SW6` will exit the DAC test.

## 6. Testing the ADC and DC Motor

- Select `6` in the Matrix keypad to test the ADC.
- This will start the ADC Program.
- ADC is connected to a Potentiometer.
- ADC output controls a DC Motor's speed.
- Rotate the potentiometer to see the change in the speed of the DC Motor.
- `SW6` will exit the program.

## 7. Testing the LCD

- Select `7` in the Matrix keypad to test the LCD.
- This will start the LCD Program.
- The LCD will display the string `RV College Of Engrng` , `Computer Sciene`,`4th Semester` and `B Section`.
- `SW6` will exit the program.
