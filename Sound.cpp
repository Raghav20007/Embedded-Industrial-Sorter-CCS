// Sound.cpp
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// your name
// your data 
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"



uint32_t tick = 0;
uint32_t DeCount = 0;




void SysTick_IntArm(uint32_t period, uint32_t priority){
  // write this
  SysTick->CTRL = 0x00;   //disabling systick clock, interrupts, and general use
  SysTick->LOAD = period-1;
  SysTick->VAL = 0;       //clears flag
  SCB->SHP[1] = (SCB->SHP[1]&(~0xC0000000))|(priority<<30); //sets priorities to msb's
  SysTick->CTRL = 0x07;   //turning systick on

}
// initialize a 11kHz SysTick, however no sound should be started
// initialize any global variables
// Initialize the 5 bit DAC
uint32_t Counter;
const uint8_t *ArrayPointer;

void Sound_Init(void){
// write this
  DAC5_Init();
  SysTick_IntArm(7273, 2);

}
extern "C" void SysTick_Handler(void);
void SysTick_Handler(void){ // called at 11 kHz
  // output one value to DAC if a sound is active
    // output one value to DAC if a sound is active
    tick++;
    DeCount = Counter;
    if(Counter > 0){
      DAC5_Out(*ArrayPointer);
      ArrayPointer++;
      Counter--;
    } else {
      SysTick->CTRL = 0x0;
    }

}

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the SysTick interrupt.
// It starts the sound, and the SysTick ISR does the output
// feel free to change the parameters
// Sound should play once and stop
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement
void Sound_Start(const uint8_t *pt, uint32_t count){
// write 
  if(count == 0){
    return;
  }

  SysTick->CTRL = 0x00;
  Counter = count;
  ArrayPointer = pt;

  SysTick->LOAD = 7272;
  SysTick->VAL = 0;        
  SysTick->CTRL = 0x07; 
}

void Sound_Shoot(void){
// write this
  Sound_Start( error, 320);
  
}
void Sound_Killed(void){
// write this

}
void Sound_Explosion(void){
// write this
  Sound_Start(error_sound_short, 1650);
}

void Sound_Fastinvader1(void){

}
void Sound_Fastinvader2(void){

}
void Sound_Fastinvader3(void){

}
void Sound_Fastinvader4(void){

}
void Sound_Highpitch(void){

}
