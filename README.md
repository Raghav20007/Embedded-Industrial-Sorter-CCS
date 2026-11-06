Embedded Systems' Industrial Sorter Game!!!!!!!!!!!

A handheld arcade-style game for the TI MSPM0G3507 LaunchPad, built in Code Composer Studio (CCS).
Nails fall down the screen into a trash can and to catch them, move the sorting arm with a slide potentiometer
to catch the quality nails while letting the defective ones fall past.

Software layout

-Lab9HMain.cpp: Main program and game logic. A 30 Hz timer interrupt (TIMG12) reads the buttons and 
slide pot, moves the arm and falling nails, and checks for catches. The main loop draws the screen 
when the interrupt signals it. Also holds the language text tables and the small test mains 
(main1 to main4).

-SPI.cpp: SPI driver for the LCD. Sets up SPI1 at about 8 MHz and provides SPI_OutData,
SPI_OutCommand and SPI_Reset, which pulses the LCD reset pin.

-SlidePot.cpp: Slide pot driver. Initializes ADC1 channel 5 (12-bit, software trigger) and returns
a 0 to 4095 reading. Also has the distance conversion and a semaphore helper.

-Switch.cpp: Initializes the two buttons on PA15 and PA17 and returns their state as a 2-bit value.

-LED.cpp:	Initializes the three LEDs on PA24 to PA26, with on, off and toggle functions.

-Sound.cpp:	Plays sounds through the 5-bit DAC. An 11 kHz SysTick interrupt sends one sample per tick
from a sound array. Sound_Shoot is the catch sound and Sound_Explosion is the error sound. 
The other Sound_* functions are empty stubs.

-Sound.h: Declares the sound functions Sound_Init, Sound_Start and holds the sound data as arrays of 
5-bit DAC values (0 to 31).

-DAC5.cpp: Driver for the 5-bit binary-weighted DAC on PB4 to PB0. DAC5_Init sets those five
pins as GPIO outputs. DAC5_Out writes a value from 0 to 31 to the pins, which the resistor network 
turns into a voltage of about n * 3.3 V / 31. The SysTick interrupt in Sound.cpp calls this once per sample.

-Clock.cpp: Sets the CPU clock speed. Clock_Init80MHz uses the PLL to run the chip at 80 MHz. Also provides
busy-wait delays (Clock_Delay, Clock_Delay1ms), SysTick wait functions, and TimerG8_Init for a free-running 
timer used for interrupts in the main game loop (This file was primarily written by instructor Jonathan Valvano).


