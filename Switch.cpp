/*
 * Switch.cpp
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
// LaunchPad.h defines all the indices into the PINCM table
void Switch_Init(void){

  IOMUX->SECCFG.PINCM[PA17INDEX] = 0x00040081; // input, no pull
  IOMUX->SECCFG.PINCM[PA15INDEX] = 0x00040081; // input, no pull
}
// return current state of switches
uint32_t Switch_In(void){

  uint32_t data = GPIOA->DIN31_0;
  data = ((data>>15)&0x01) | ((data>>16)&0x02);
  return data; 
}
