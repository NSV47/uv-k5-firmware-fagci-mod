#include "helloWorld.h"
#include "../driver/eeprom.h"
#include "../helper/measurements.h"
#include "../misc.h"
#include "../settings.h"
#include "../ui/ui.h"
#include <stdio.h>
#include "../scheduler.h"
#include "../app/aircopy.h"
// #include "../driver/bk4819.h"

// cw dot length in ms
// #define CW_DOT_LEN 120

#define RECEIVER

uint16_t my_FSK_Buffer[36];

#define flag_action_1 (1 << 0)  // 0b00000001
#define flag_action_2 (1 << 1)  // 0b00000010

uint8_t flag_action = 0;

#if 0
  // Проверка флага
if (gFlags & FLAG_2000MS) {
    // Действие
}

// Сброс флага
gFlags &= ~FLAG_2000MS;
#endif

// bool flag_action = true;
// bool flag_action2 = false;

#ifndef RECEIVER
  uint32_t timer_1=0;
#endif
uint32_t timer_2=0;

#ifdef RECEIVER
uint8_t mygFSKWriteIndex=0;
void APP_myCheckRadioInterrupts(void) {

  while (BK4819_ReadRegister(BK4819_REG_0C) & 1U) {
    
    // uint16_t Mask;

    // BK4819_WriteRegister(BK4819_REG_02, 0);
    // Mask = BK4819_ReadRegister(BK4819_REG_02);
    // UART_Send("s:", 2); 
    // uint16_t res;
    // res = Mask & BK4819_REG_02_FSK_FIFO_ALMOST_FULL;
    // UART_Send((const void *)&Mask, sizeof(Mask)); 
    // UART_Send("\r\n", 2);
    // UART_Send((const void *)&res, sizeof(res));
    // UART_Send("\r\n", 2);
    for (uint8_t i = 0; i < 4; i++) {
      my_FSK_Buffer[mygFSKWriteIndex++] = BK4819_ReadRegister(BK4819_REG_5F);
      // uint16_t value = BK4819_ReadRegister(BK4819_REG_5F);
      // UART_Send((const void *)&value, 1);
    }
    // if (Mask & BK4819_REG_02_FSK_FIFO_ALMOST_FULL/* &&
        // gScreenToDisplay == DISPLAY_AIRCOPY &&
        // gAircopyState == AIRCOPY_TRANSFER && gAirCopyIsSendMode == 0*/) 
      // {
      // UART_Send("1", 1);    
      // uint8_t i;
      

      // for (uint8_t i = 0; i < 4; i++) {
        // my_FSK_Buffer[mygFSKWriteIndex++] = BK4819_ReadRegister(BK4819_REG_5F);
        // uint16_t value = BK4819_ReadRegister(BK4819_REG_5F);
        // UART_Send((const void *)&value, 1);
      // }
      // AIRCOPY_StorePacket();
      // mygFSKWriteIndex=0;
      // uint16_t Status;
      // Status = BK4819_ReadRegister(BK4819_REG_0B);
      // UART_Send((const void *)&Status, 1);
      // UART_Send("\r\n", 2);
      // UART_Send("Ok\r\n", 4);
      // BK4819_PrepareFSKReceive();
      // const void *pBuffer = (const void *)&my_FSK_Buffer[0];
      // UART_Send(pBuffer, sizeof(&my_FSK_Buffer[0]));
      // for(uint8_t i=0;i<36;++i){
        // UART_Send((const void *)&my_FSK_Buffer[i], 1);
      // }
      // if ((Status & 0x0010U) == 0/* && g_FSK_Buffer[0] == 0xABCD && g_FSK_Buffer[35] == 0xDCBA */) {
        // UART_Send(my_FSK_Buffer, sizeof(my_FSK_Buffer));
      // }
      // flag_action2=true;
      // Установка флага
      // flag_action |= flag_action_2;
      flag_action &= ~flag_action_2;

    // }

  }
}
#endif
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
#ifdef RECEIVER // -> компилируется приёмник
  
  #if 0 // app.c строка 581 сначала заполняем буфер, потом StorePacket()
    #if defined(ENABLE_AIRCOPY)
      if (Mask & BK4819_REG_02_FSK_FIFO_ALMOST_FULL &&
          gScreenToDisplay == DISPLAY_AIRCOPY &&
          gAircopyState == AIRCOPY_TRANSFER && gAirCopyIsSendMode == 0) {
        uint8_t i;

        for (i = 0; i < 4; i++) {
          g_FSK_Buffer[gFSKWriteIndex++] = BK4819_ReadRegister(BK4819_REG_5F);
        }
        AIRCOPY_StorePacket();
      }
    #endif
  #endif
  if(millis()-timer_2>=1){
    timer_2=millis();
    APP_myCheckRadioInterrupts();
  }
  // if(flag_action2){
    // flag_action2=false;
  // }
  // Проверка флага
  if (!(flag_action & flag_action_2)) {
    // Действие
    // Сброс флага
    // flag_action &= ~flag_action_2;
    flag_action |= flag_action_2;
    for(uint8_t i=0;i<36;++i){
      UART_Send((const void *)&my_FSK_Buffer[i], 1);
    }
  }
#endif    
  //-----------------------------------------------------------------------------
#ifndef RECEIVER // -> компилируется передатчик
  // cwTx("NSV");
  if(millis()-timer_1>=500){
    // timer_1=millis();
    // cwTx("NSV");
    
    // memset(my_FSK_Buffer,'1',36); // заполнить первые 12 байт символом '_'
    
    

  //-----------------------------------------------------------------------------  
	  // gAircopySendCountdown = 30;

    // uint32_t buff = millis();
    // const void *pBuffer = (const void *)&buff;
    // UART_Send(pBuffer, sizeof(buff));
  }
  // if(flag_action){
    // flag_action=false;
  // Проверка флага
  if (flag_action & flag_action_1) {
    // Сброс флага
    flag_action &= ~flag_action_1;
    for(uint8_t i=0;i<36;++i){
      my_FSK_Buffer[i]=i+1;
    }

    my_FSK_Buffer[0] = 0xABCD;
    // my_FSK_Buffer[1] = (gAirCopyBlockNumber & 0x3FF) << 6;
    my_FSK_Buffer[35] = 0xDCBA;
    for(uint8_t i=0;i<10;i++){
      RADIO_enableTX();
      BK4819_SendFSKData(my_FSK_Buffer);
      BK4819_SetupPowerAmplifier(0, 0);
      BK4819_ToggleGpioOut(BK4819_GPIO1_PIN29_PA_ENABLE, false);
    }
  }
#endif
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
    // case KEY_MENU:
    case KEY_EXIT:
      gAppToDisplay = APP_SPLIT;
      gRequestDisplayScreen = DISPLAY_MAIN;
      // flag_action=true;
      // Установка флага
      flag_action |= flag_action_1;
      #ifdef RECEIVER
        mygFSKWriteIndex=0;
        BK4819_PrepareFSKReceive();
        // Установка флага
        flag_action |= flag_action_2;
      #endif
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
  // UI_PrintStringSmallest("Hello World app", 0, 32, false, true);

  ST7565_BlitFullScreen();
}
