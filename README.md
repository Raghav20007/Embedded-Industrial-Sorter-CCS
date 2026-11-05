Embedded Systems' Industrial Sorter!!!!!!!!!!!

A handheld arcade-style game for the TI MSPM0G3507 LaunchPad, built in Code Composer Studio (CCS).
Nails fall down the screen on a conveyor. Move the sorting arm with a slide potentiometer
to catch the good ones and let the defective ones fall past.

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

-SmallFont.cpp:	Draws small 4-digit numbers on the LCD as bitmaps, in vertical or horizontal orientation.
