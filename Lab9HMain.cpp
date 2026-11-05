// Lab9HMain.cpp
// Runs on MSPM0G3507

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/SlidePot.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "images/images.h"
extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);

struct Item {
  int32_t x;
  int32_t y;
  int32_t oldY;       // Used to erase the previous frame
  uint8_t active;     // 1 = falling, 0 = waiting to spawn
  uint8_t isPristine; // 1 = avoid, 0 = catch
};

Item  FallingItem[5];
int32_t ArmX = 54;    
int32_t OldArmX = 54;
int32_t Score = 0;
int32_t Lives = 5;

// 0 = playin, 1 = paused, 2 = game over, 3 = main menu
uint8_t GameState = 3;    
uint32_t LastSwitches = 0;
uint32_t NeedToDraw = 0;   // Semaphore
uint32_t flag = 0;
// Fixed: Expanded arrays to hold 4 strings to match the 4-language enum
const char *Score_Text[4] = {"Scr:", "Pts:", "Pts:", "Pts:"};
const char *Lives_Text[4] = {" Lvs:", " Vid:", " Vid:", " Vie:"};
const char *Paused_Text[4] = {" PAUSED ", " PAUSA  ", " PAUSA  ", " PAUSE  "};
const char *Fail_Text[4] = {"SYSTEM FAILURE", "FALLO SISTEMA ", "FALHA SISTEMA ", "ECHEC SYSTEME "};
const char *Final_Text[4] = {"Final Score: ", "Puntaje final:", "Pontos Finais:", "Score Final: "};
const char *Press_Text[4] = {"Press Play to Start", "Presione Play p/ Iniciar", "Pressione Play", "Appuyez sur Play"};
const char *Reset_Text[4] = {"Press Reset", "Presione Rst ", "Pressione Rst", "Appuyez sur Rst"};


void PLL_Init(void){ // set phase lock loop (PLL)
//  Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

uint32_t M=1;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}

SlidePot Sensor(1500,0); 

// games engine runs at 30Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
  GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)

  // button 1 (play/pause): bit 0
  // button 2 (reset): bit 1:
  uint32_t NowSwitches = Switch_In();

  // pause/play button logic
  if(((LastSwitches & 1) == 0) && ((NowSwitches & 1) == 1)) {
    if(GameState == 3) {
      GameState = 0; // Start game from menu
    }
    else if(GameState == 0) GameState = 1;
    else if(GameState == 1) GameState = 0;
  }

  // reset game state logic
  if(((LastSwitches & 2) == 0) && ((NowSwitches & 2) == 2)) {
    Score = 0;
    Lives = 20;
    GameState = 3; // Go back to main menu
    for(int i = 0; i < 5; i++) FallingItem[i].active = 0;
  }

  LastSwitches = NowSwitches;

  // check gameover
  if(Lives <= 0) {
    GameState = 2;
  }

  // play logic
  if(GameState == 0) {
      // move arm to new position
      OldArmX = ArmX;
      // slidepot reading
      uint32_t adcValue = Sensor.In();
      ArmX = (adcValue * 108) / 4095; // 128 screen width - 20 arm width
     
      // iterate through items
      for(int i = 0; i < 5; i++){
        if(FallingItem[i].active == 1){
          FallingItem[i].oldY = FallingItem[i].y;
          FallingItem[i].y += 3; // fall speed
         
          // collision check (Arm is at Y=140, height 10)
          if(FallingItem[i].y >= 140){
            // horizontal hit detection
            if((FallingItem[i].x + 7 >= ArmX) && (FallingItem[i].x <= ArmX + 20)){
              if(FallingItem[i].isPristine == 0){
                Score++;
                Sound_Shoot(); // clank sound
              } else {
                Lives--;
                Sound_Explosion(); // error sound
              }
            } else {
              // missed it
              if(FallingItem[i].isPristine == 0){
                Lives--;
                Sound_Explosion(); // error sound
              }
            }
            FallingItem[i].active = 0; // kill item after it hits bottom
          }
        }
        else {
          // spawning logic (3% chance per frame per slot)
          if(Random32() % 100 < 3){
            FallingItem[i].x = Random32() % 121;
            FallingItem[i].y = 0;                
            FallingItem[i].oldY = 0;
            FallingItem[i].isPristine = Random32() % 2;
            FallingItem[i].active = 1;
          }
        }
      }
    }
    NeedToDraw = 1; // signal the main loop
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
   }
  }

uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}

// Single, correct language enum definition
typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};

// Renamed from main to main1
int main1(void){ // main1
    char l;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_REDTAB); // INITR_REDTAB for AdaFruit, INITR_BLACKTAB for HiLetGo
  ST7735_FillScreen(0x0000);            // set screen to black
  for(int myPhrase=0; myPhrase<= 2; myPhrase++){
    for(int myL=0; myL<= 3; myL++){
         ST7735_OutString((char *)Phrases[LANGUAGE][myL]);
      ST7735_OutChar(' ');
         ST7735_OutString((char *)Phrases[myPhrase][myL]);
      ST7735_OutChar(13);
    }
  }
  Clock_Delay1ms(3000);
  ST7735_FillScreen(0x0000);       // set screen to black
  l = 128;
  while(1){
    Clock_Delay1ms(2000);
    for(int j=0; j < 3; j++){
      for(int i=0;i<16;i++){
        ST7735_SetCursor(7*j+0,i);
        ST7735_OutUDec(l);
        ST7735_OutChar(' ');
        ST7735_OutChar(' ');
        ST7735_SetCursor(7*j+4,i);
        ST7735_OutChar(l);
        l++;
      }
    }
  }
}

// use main2 to observe graphics
int main2(void){ // main2
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_REDTAB); // INITR_REDTAB for AdaFruit, INITR_BLACKTAB for HiLetGo
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_DrawBitmap(22, 159, PlayerShip0, 18,8); // player ship bottom
  ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
  ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
  ST7735_DrawBitmap(62, 159, PlayerShip2, 18,8); // player ship bottom
  ST7735_DrawBitmap(82, 159, PlayerShip3, 18,8); // player ship bottom
  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);

  for(uint32_t t=500;t>0;t=t-5){
    SmallFont_OutVertical(t,104,6); // top left
    Clock_Delay1ms(50);              // delay 50 msec
  }
  ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString((char *)"GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString((char *)"Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString((char *)"Earthling!");
  ST7735_SetCursor(2, 4);
  ST7735_OutUDec(1234);
  while(1){
  }
}

// use main3 to test switches and LEDs
int main3(void){ // main3
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  while(1){
    // write code to test switches and LEDs

  }
}
// use main4 to test sound outputs
int main4(void){ uint32_t last=0,now;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(ADC0,6,0); // ADC1 channel 6 is PB20, TExaS scope
  __enable_irq();
  Sound_Shoot();
  Sound_Shoot();
  while(1){
    now = Switch_In(); // one of your buttons
    if((last == 0)&&(now == 1)){
      Sound_Shoot(); // call one of your sounds
    }
    if((last == 0)&&(now == 2)){
      Sound_Explosion(); // call one of your sounds
    }
    if((last == 0)&&(now == 4)){
      Sound_Explosion(); // call one of your sounds
    }
    if((last == 0)&&(now == 8)){
      Sound_Fastinvader1(); // call one of your sounds
    }
    // modify this to test all your sounds
  }
}

// Renamed from main5 to main to run the actual game
int main(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_REDTAB); // INITR_REDTAB for AdaFruit, INITR_BLACKTAB for HiLetGo
  ST7735_FillScreen(ST7735_BLACK);
  Sensor.Init(); // PB18 = ADC1 channel 5, slidepot
  Switch_Init(); // initialize switches
  LED_Init();    // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26); // PB27 and PB26
//  Sound_Shoot();
//  Sound_Explosion();
 
  // Fixed timer logic: 80MHz clock
   / 30Hz = 2,666,666 period
  TimerG12_IntArm(2666666, 1);
 
  // initialize all data structures
  __enable_irq();

  // State tracker to handle LCD clears safely
  uint8_t OldGameState = 3;

  while(1){
    while(NeedToDraw == 0){};
    NeedToDraw = 0;

    // Clear screen safely outside of the interrupt if state changes
    if(OldGameState != GameState) {
       ST7735_FillScreen(ST7735_BLACK);
       OldGameState = GameState;
       OldArmX = -1; // Force arm to draw completely fresh
    }

    if(GameState == 3) {
      // main menu
      if(Sensor.In() < 2048) {
          myLanguage = English;
      } else {
          myLanguage = Spanish;
      }

      ST7735_SetCursor(1, 2);
      ST7735_OutString((char *)"INDUSTRIAL SORTER");

      ST7735_SetCursor(2, 6);
      if(myLanguage == English) ST7735_OutString((char *)"> English <");
      else ST7735_OutString((char *)"  English  ");
     
      ST7735_SetCursor(2, 8);
      if(myLanguage == Spanish) ST7735_OutString((char *)"> Espanol <");
      else ST7735_OutString((char *)"  Espanol  ");
     
      ST7735_SetCursor(1, 12);
      ST7735_OutString((char *)Press_Text[myLanguage]);
     
    } else if(GameState == 0) {
      // draw new arm
//      ST7735_DrawBitmap(0, 159, Background, 128, 160);
      
    if(OldArmX != ArmX){
      ST7735_FillRect(OldArmX, 140, 20, 10, ST7735_BLACK);
    }
      ST7735_DrawBitmap(ArmX, 149, arm, 20, 10);
 

      // draw items
      for(int i = 0; i < 5; i++) {
        if(FallingItem[i].active == 1) {
          if(FallingItem[i].oldY != FallingItem[i].y) {
            // erase only the top strip left behind (fall speed = 3 pixels)
            ST7735_FillRect(FallingItem[i].x, FallingItem[i].oldY, 7, FallingItem[i].y - FallingItem[i].oldY, ST7735_BLACK);
            
            // draw sprite at new position
            if(FallingItem[i].isPristine == 0) {
                ST7735_DrawBitmap(FallingItem[i].x, FallingItem[i].y + 11, NailGreen, 7, 11);
            } else {
                ST7735_DrawBitmap(FallingItem[i].x, FallingItem[i].y + 11, NailRed, 7, 11);
            }
        }
        } else if(FallingItem[i].oldY != -1) {
          ST7735_FillRect(FallingItem[i].x, FallingItem[i].oldY, 7, 11, ST7735_BLACK);
          FallingItem[i].oldY = -1;
        }
      }

      // draw HUD based on selected language
      ST7735_SetCursor(0, 0);
      ST7735_OutString((char *)Score_Text[myLanguage]);
      ST7735_OutUDec(Score);
      ST7735_OutString((char *)Lives_Text[myLanguage]);
      ST7735_OutUDec(Lives);
      ST7735_OutString((char *)"   "); // clear leftover digits
     
    } else if(GameState == 1) {
      // paused
      ST7735_SetCursor(4, 7);
      ST7735_OutString((char *)Paused_Text[myLanguage]);
     
    } else if(GameState == 2) {
      // gameover
      ST7735_SetCursor(1, 5);
      ST7735_OutString((char *)Fail_Text[myLanguage]);
      ST7735_SetCursor(1, 7);
      ST7735_OutString((char *)Final_Text[myLanguage]);
      ST7735_OutUDec(Score);
      ST7735_SetCursor(1, 9);
      ST7735_OutString((char *)Reset_Text[myLanguage]);
    }
  }
}
