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
#include "../driver/crc.h"

// #define TRANSMITTER
#define RECEIVER


uint32_t timer_1 = 0;

#if defined(TRANSMITTER) || defined(RECEIVER)
uint8_t counter=0;
#endif
#if 0 // #ifdef TRANSMITTER
static const uint16_t Obfuscation[8] = { 0x6C16, 0xE614, 0x912E, 0x400D, 0x3521, 0x40D5, 0x0313, 0x80E9 };
#endif
uint16_t my_g_FSK_Buffer[36];

uint8_t mygFSKWriteIndex = 0;

bool flag = false;

#ifdef TRANSMITTER
void my_AIRCOPY_SendMessage(void)
{
#if 0  
  my_g_FSK_Buffer[0] = 0xABCD;
  my_g_FSK_Buffer[1] = 0;
  my_g_FSK_Buffer[35] = 0xDCBA;

	// my_g_FSK_Buffer[1] = 0x31; // (gAirCopyBlockNumber & 0x3FF) << 6;
	EEPROM_ReadBuffer(my_g_FSK_Buffer[1], &my_g_FSK_Buffer[2], 64);
	my_g_FSK_Buffer[34] = CRC_Calculate(&my_g_FSK_Buffer[1], 2 + 64);
	for (uint8_t i = 0; i < 34; i++) {
		my_g_FSK_Buffer[i + 1] ^= Obfuscation[i % 8];
	}
#endif
#if 1 
  for (uint8_t i = 0; i < 36; i++) {
    my_g_FSK_Buffer[i]=(uint16_t)i+1;
  }
#endif
	RADIO_enableTX();
	BK4819_SendFSKData(my_g_FSK_Buffer);
	BK4819_SetupPowerAmplifier(0, 0);
	BK4819_ToggleGpioOut(BK4819_GPIO1_PIN29_PA_ENABLE, false);
}
#endif

void print_StorePacket(){
  uint16_t myStatus;
  myStatus = BK4819_ReadRegister(BK4819_REG_0B);
  UART_Send((const void *)&myStatus, 1);
	BK4819_PrepareFSKReceive();
  for(uint8_t i =0;i<36;++i){
    UART_Send((const void *)&my_g_FSK_Buffer[i], 1);
  }
  mygFSKWriteIndex=0;
  // counter=0;
}

#ifdef RECEIVER
void my_APP_CheckRadioInterrupts(void) {

  while (BK4819_ReadRegister(BK4819_REG_0C) & 1U) {
    volatile uint16_t Mask;

    BK4819_WriteRegister(BK4819_REG_02, 0);
    Mask = BK4819_ReadRegister(BK4819_REG_02);

    UART_Send((const void *)&Mask, sizeof(Mask));
    UART_Send("\r", 1);
    uint16_t res;
    res = Mask & BK4819_REG_02_FSK_FIFO_ALMOST_FULL;
    
    UART_Send((const void *)&res, sizeof(res));
    UART_Send("\r", 1);

    if (Mask & (1<<3)) {
      flag=true;
    }

    if (Mask & (1<<12)) { // BK4819_REG_02_FSK_FIFO_ALMOST_FULL // (uint16_t)(1<<3)
      UART_Send("ok", 2);
      for (uint8_t i = 0; i < 4; i++) {
        my_g_FSK_Buffer[mygFSKWriteIndex++] = BK4819_ReadRegister(BK4819_REG_5F);
      }
      flag=true;
    }
  }
}
#endif

void HELLOWORLD_update() {
  
  if(millis()-timer_1>=1){
    timer_1=millis();
  
  //--------------------------------------------------------------------
  #ifdef RECEIVER
    if(counter<=0){
      counter++;
      RADIO_InitInfo(gRxVfo, 205, 5, 41002500);
      gRxVfo->CHANNEL_BANDWIDTH = BANDWIDTH_NARROW;
      gRxVfo->OUTPUT_POWER = 0;
      RADIO_ConfigureSquelchAndOutputPower(gRxVfo);
      gCurrentVfo = gRxVfo;
      RADIO_SetupRegisters(true);
      BK4819_SetupAircopy();
      // BK4819_ResetFSK();
    }
    my_APP_CheckRadioInterrupts();
    if(flag){
      flag=false;
      print_StorePacket();
    }
    #endif  
  //--------------------------------------------------------------------  
  #ifdef TRANSMITTER
    if(counter<=0){
      counter++;
      RADIO_InitInfo(gRxVfo, 205, 5, 41002500);
      gRxVfo->CHANNEL_BANDWIDTH = BANDWIDTH_NARROW;
      gRxVfo->OUTPUT_POWER = 0;
      RADIO_ConfigureSquelchAndOutputPower(gRxVfo);
      gCurrentVfo = gRxVfo;
      RADIO_SetupRegisters(true);
      BK4819_SetupAircopy();
      BK4819_ResetFSK();
      my_AIRCOPY_SendMessage();
    }
    #endif
  }
  // uint16_t value = BK4819_ReadRegister(BK4819_REG_58);
  // UART_Send((const void *)&value, sizeof(value));
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
      #ifdef TRANSMITTER
      counter=0;
      #endif
      mygFSKWriteIndex=0;
      break;
    default:
      break;
    }
    gUpdateDisplay = true;
  }
}

void HELLOWORLD_render() {

  UI_ClearAppScreen();

  // UI_PrintStringSmallest("Hello World app", 0, 32, false, true);

  ST7565_BlitFullScreen();
}
