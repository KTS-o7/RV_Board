# RV_Board

This is the Repository of RV Board Testing program Development

## 1. Introduction

The code must be run on the Keil uVision5 IDE. The code is written in C language. The code is developed for RV Board. The RV Board is a development board for the LPC2148.

## 2. Features

The RV Board has the following features:

- 2x16 LCD Display.
- 4x4 Keypad.
- 8 LEDs.
- 8 Switches.
- 1 Potentiometer.
- 1 Buzzer.
- Servo Motor outputs.
- Stepper Motor outputs.
- DC Motor outputs.
- 1 Relay.
- 10 bit ADC x 2 ( A total of 14 ADC channels are available ).
- 10 Bit DAC x 1 ( A single `Aout` pin is available ).
- 2 UARTs.
- 2 I2C.
- 1 SPI.
- 512KB Flash and 32KB RAM.
- 2 Timers.
- 2 External Interrupts ( FIQ and IRQ ).
- 1 PWM with 6 channels.

## 3. How to use

`Programming mode` can be accessed by pressing `SW2` and `Reset` Switches together. The `Programming mode` is indicated by the `LED1` glowing. The `Programming mode` is used to program the LPC2148 using the `ISP` mode. The `Programming mode` is exited by pressing the `Reset` switch.

## 4. How to program

Burn the `<program name>.hex` via FlashMagic.
Select the appropriate COM port and the baud rate.

## Productathon Code

- A version of Smart Kiosk is written in Embedded C which utilizes the features of the RV Board that is - 4x4 Matrix Keyboard, LCD, Buzzer and DC Motor.

Compile the code with microLib, appropriate linker file and startup file and the default memorylayout.

@KTS-o7,@NeeleshRao,@CodingRiovo, @Sarangspin and @using-namespace-lyx contributed to this repository.
