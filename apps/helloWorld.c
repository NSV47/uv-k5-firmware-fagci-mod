#include "helloWorld.h"
#include "../driver/eeprom.h"
#include "../helper/measurements.h"
#include "../misc.h"
#include "../settings.h"
#include "../ui/ui.h"
#include <stdio.h>
#include "../scheduler.h"

// cw dot length in ms
#define CW_DOT_LEN 120

// static uint16_t cursor;
uint32_t timer_1=0;

const uint8_t cwSymbTab[][5] = {
  {1, 2},             // 0  A
  {2, 1, 1, 1},       // 1  B
  {2, 1, 2, 1},       // 2  C
  {2, 1, 1},          // 3  D
  {1},                // 4  E
  {1, 1, 2, 1},       // 5  F
  {2, 2, 1},          // 6  G
  {1, 1, 1, 1},       // 7  H
  {1, 1},             // 8  I
  {1, 2, 2, 2},       // 9  J
  {2, 1, 2},          // 10 K
  {1, 2, 1, 1},       // 11 L
  {2, 2},             // 12 M
  {2, 1},             // 13 N
  {2, 2, 2},          // 14 O
  {1, 2, 2, 1},       // 15 P
  {2, 2, 1, 2},       // 16 Q
  {1, 2, 1},          // 17 R
  {1, 1, 1},          // 18 S
  {2},                // 19 T
  {1, 1, 2},          // 20 U
  {1, 1, 1, 2},       // 21 V
  {1, 2, 2},          // 22 W
  {2, 1, 1, 2},       // 23 X
  {2, 1, 2, 2},       // 24 Y
  {2, 2, 1, 1},       // 25 Z
  {2, 2, 2, 2, 2},    // 26 0
  {1, 2, 2, 2, 2},    // 27 1
  {1, 1, 2, 2, 2},    // 28 2
  {1, 1, 1, 2, 2},    // 29 3
  {1, 1, 1, 1, 2},    // 30 4
  {1, 1, 1, 1, 1},    // 31 5
  {2, 1, 1, 1, 1},    // 32 6
  {2, 2, 1, 1, 1},    // 33 7
  {2, 2, 2, 1, 1},    // 34 8
  {2, 2, 2, 2, 1}     // 35 9
};

void cwSendSym(uint16_t len)
{
  // si5351.output_enable(SI5351_CLK0, 1);
  // gen.EnableOutput(true);   // Turn ON the output - it defaults to OFF
  // digitalWrite(2,HIGH);
  RADIO_enableTX();
  BK4819_TransmitTone(true, 1200);
  // delay(len);
  SYSTEM_DelayMs(len);
  // si5351.output_enable(SI5351_CLK0, 0);  
  // gen.EnableOutput(false);   // Turn ON the output - it defaults to OFF
  // digitalWrite(2,LOW);
  RADIO_disableTX();
}

void cwTxChar(char ch)
{
    uint8_t cwSym;
    uint8_t tabIndex;
    
    tabIndex = 255;
    if ((ch >= 65) && (ch <= 90))  tabIndex = ch - 65;  // A - Z
    if ((ch >= 97) && (ch <= 122)) tabIndex = ch - 97;  // a - z
    if ((ch >= 48) && (ch <= 57))  tabIndex = ch - 22;  // 0 - 9

    if (tabIndex == 255)
    {
      SYSTEM_DelayMs(CW_DOT_LEN * 3);
      return;
    }
    
    for(uint8_t i = 0; i < 5; i++) 
    {
      cwSym = cwSymbTab[tabIndex][i];
      if      (cwSym == 1) cwSendSym(CW_DOT_LEN);
      else if (cwSym == 2) cwSendSym(CW_DOT_LEN * 3);
      else continue;
      SYSTEM_DelayMs(CW_DOT_LEN);
    }

    SYSTEM_DelayMs(CW_DOT_LEN * 2);
}

void cwTx(char* msg)
{
  // digitalWrite(6,HIGH);
  for(uint8_t i = 0; i < strlen(msg); i++) {
    cwTxChar(msg[i]);
  }
  // digitalWrite(6,LOW );
}

void helloWorldUpdate(uint8_t Channel, uint8_t val) {
#if 0  
  uint8_t State[8];
  uint16_t Offset = 0x0D60 + (Channel & ~7U);

  bool s1 = val & 1;
  bool s2 = val & 2;

  gMR_ChannelAttributes[Channel] &= ~(MR_CH_SCANLIST1 | MR_CH_SCANLIST2);
  if (s1) {
    gMR_ChannelAttributes[Channel] |= MR_CH_SCANLIST1;
  }
  if (s2) {
    gMR_ChannelAttributes[Channel] |= MR_CH_SCANLIST2;
  }

  EEPROM_ReadBuffer(Offset, State, sizeof(State));
  State[Channel & 7U] = gMR_ChannelAttributes[Channel];
  EEPROM_WriteBuffer(Offset, State);
#endif  
  // BK4819_PlayTone(1200, true);
  // RADIO_enableTX();
  // BK4819_TransmitTone(true, 1200);
  // uint16_t buff[4]={'t','e','s','t'};
  // BK4819_SendFSKData(buff);
  // BK4819_SetupPowerAmplifier(0, 0);
  // BK4819_ToggleGpioOut(BK4819_GPIO1_PIN29_PA_ENABLE, false);
}

void HELLOWORLD_update() {
  // counter++;
#if 0  
  uint32_t primask = __get_PRIMASK();;
  __disable_irq();
  //--critical section?--
  
  //---------------------
  __set_PRIMASK(primask);
  
#endif  
  
  // cwTx("NSV");
  if(millis()-timer_1>=500){
    timer_1=millis();
    cwTx("NSV");
    // uint32_t buff = millis();
    // const void *pBuffer = (const void *)&buff;
    // UART_Send(pBuffer, sizeof(buff));
  }
  // SYSTEM_DelayMs(2 * 1000);
  // HELLOWORLD_key();
  // if(gNextTimeslice4000ms){
    // cwTx("NSV");
    // gNextTimeslice4000ms = false;
  // }
  
}

void HELLOWORLD_key(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld) {
  if (bKeyPressed) {
    switch (Key) {
  #if 0  
      case KEY_DOWN:
      if (cursor < 199) {
        cursor++;
      } else {
        cursor = 0;
      }
      break;
  #endif
  #if 0
    case KEY_UP:
      if (cursor > 0) {
        cursor--;
      } else {
        cursor = 199;
      }
      break;
  #endif
  #if 0
    case KEY_0:
    case KEY_1:
    case KEY_2:
    case KEY_3:
      helloWorldUpdate(cursor, Key - KEY_0);
      break;
  #endif    
    case KEY_MENU:
    case KEY_EXIT:
      gAppToDisplay = APP_SPLIT;
      gRequestDisplayScreen = DISPLAY_MAIN;
      break;
    default:
      break;
    }
    gUpdateDisplay = true;
  }
}

void HELLOWORLD_render() {
#if 0  
  char String[32];
  char channelName[16];
  UI_ClearAppScreen();

  const uint8_t count = 200;
  const uint8_t perScreen = 3;
  const uint8_t offset = Clamp(cursor - 2, 0, count - perScreen);
  for (uint8_t i = 0; i < perScreen; ++i) {
    uint8_t itemIndex = i + offset;
    uint8_t chPos = 4 + i;
    uint8_t chNum = itemIndex + 1;
    uint8_t y = 33 + i * 8;
    uint8_t *pLine = gFrameBuffer[chPos];

    bool isCurrent = cursor == i + offset;

    if (isCurrent) {
      memset(pLine, 127, LCD_WIDTH);
    }

    GetChannelName(itemIndex, channelName);

    if (UI_NoChannelName(channelName)) {
      sprintf(channelName, "CH-%03u", chNum);
    }
    sprintf(String, "%03u: %s", chNum, channelName);

    UI_PrintStringSmallest(String, 1, y, false, !isCurrent);

    UI_DrawScanListFlag(pLine, gMR_ChannelAttributes[itemIndex]);
  
  }
#endif

  UI_ClearAppScreen();
#if 0  
  for (uint8_t line = 2; line < 7; line++) {
    memset(gFrameBuffer[line], 0, LCD_WIDTH);
  }
#endif
  UI_PrintStringSmallest("Hello World app", 0, 32, false, true);

  ST7565_BlitFullScreen();
}
