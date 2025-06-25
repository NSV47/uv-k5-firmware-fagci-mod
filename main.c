/* Copyright 2023 Dual Tachyon
 * https://github.com/DualTachyon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include "app/app.h"
#include "app/dtmf.h"
#include "audio.h"
#if defined(ENABLE_AM_FIX)
#include "am_fix.h"
#endif
#include "board.h"
#include "bsp/dp32g030/gpio.h"
#include "bsp/dp32g030/syscon.h"
#include "driver/backlight.h"
#include "driver/bk4819.h"
#include "driver/gpio.h"
#include "driver/system.h"
#include "driver/systick.h"
#include <string.h>
#if defined(ENABLE_UART)
#include "driver/uart.h"
#endif
#include "helper/battery.h"
#include "helper/boot.h"
#include "misc.h"
#include "radio.h"
#include "settings.h"
#include "ui/lock.h"
#include "ui/menu.h"
#include "ui/welcome.h"
#include "version.h"

// cw dot length in ms
#define CW_DOT_LEN 120

void _putchar(char c) {
#if defined(ENABLE_UART)
  UART_Send((uint8_t *)&c, 1);
#endif
}

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

void Main(void) {
  uint8_t i;

  // Enable clock gating of blocks we need.
  SYSCON_DEV_CLK_GATE = 0 | SYSCON_DEV_CLK_GATE_GPIOA_BITS_ENABLE |
                        SYSCON_DEV_CLK_GATE_GPIOB_BITS_ENABLE |
                        SYSCON_DEV_CLK_GATE_GPIOC_BITS_ENABLE |
                        SYSCON_DEV_CLK_GATE_UART1_BITS_ENABLE |
                        SYSCON_DEV_CLK_GATE_SPI0_BITS_ENABLE |
                        SYSCON_DEV_CLK_GATE_SARADC_BITS_ENABLE |
                        SYSCON_DEV_CLK_GATE_CRC_BITS_ENABLE |
                        SYSCON_DEV_CLK_GATE_AES_BITS_ENABLE;

  SYSTICK_Init();
  BOARD_Init();

#if defined(ENABLE_UART)
  UART_Init();
  UART_Send(UART_Version, sizeof(UART_Version));
#endif

  // Not implementing authentic device checks

  memset(&gEeprom, 0, sizeof(gEeprom));
  memset(gDTMF_String, '-', sizeof(gDTMF_String));
  gDTMF_String[14] = 0;

  BK4819_Init();
  BOARD_ADC_GetBatteryInfo(&gBatteryCurrentVoltage, &gBatteryCurrent);
  BOARD_EEPROM_Init();
  BOARD_EEPROM_LoadCalibration();

  RADIO_ConfigureChannel(0, 2);
  RADIO_ConfigureChannel(1, 2);
  RADIO_SelectVfos();
  RADIO_SetupRegisters(true);

  for (i = 0; i < 4; i++) {
    BOARD_ADC_GetBatteryInfo(&gBatteryVoltages[i], &gBatteryCurrent);
  }

  BATTERY_GetReadings(false);
#ifdef ENABLE_AM_FIX
  AM_fix_init();
#endif
  if (!gChargingWithTypeC && !gBatteryDisplayLevel) {
    FUNCTION_Select(FUNCTION_POWER_SAVE);
    GPIO_ClearBit(&GPIOB->DATA, GPIOB_PIN_BACKLIGHT);
    gReducedService = true;
  } else {
    BOOT_Mode_t BootMode;

    UI_DisplayWelcome();
    BACKLIGHT_TurnOn();
    SYSTEM_DelayMs(1000);
    gMenuListCount = MENU_ITEMS_COUNT - 6;

    BootMode = BOOT_GetMode();
    if (gEeprom.POWER_ON_PASSWORD < 1000000) {
      bIsInLockScreen = true;
      UI_DisplayLock();
      bIsInLockScreen = false;
    }

    BOOT_ProcessMode(BootMode);

    gUpdateStatus = true;
  }
  // RADIO_enableTX();
  // BK4819_TransmitTone(true, 1200);
  //---------------------------------------------------------------------------
  // RADIO_enableTX();
  // BK4819_TransmitTone(true, 1200);
  // SYSTEM_DelayMs(1000);
  // RADIO_disableTX();
  //---------------------------------------------------------------------------
  // cwSendSym(1000);
  while (1) {
#if 0  
    APP_Update();
    if (gNextTimeslice) {
      APP_TimeSlice10ms();
      gNextTimeslice = false;
    }
    if (gNextTimeslice500ms) {
      APP_TimeSlice500ms();
      gNextTimeslice500ms = false;
    }
#endif      

    cwTx("NSV");
    SYSTEM_DelayMs(2 * 1000);
  }  
}
