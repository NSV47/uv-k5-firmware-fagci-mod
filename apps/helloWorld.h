#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include "../driver/keyboard.h"
#include "../driver/st7565.h"
#include "../external/printf/printf.h"
#include "../ui/helper.h"
#include <string.h>
#include "../driver/bk4819.h"

//---------------------------------------------------------------
// #include <string.h>
// #if !defined(ENABLE_OVERLAY)
// #include "../external/CMSIS_5/Device/ARM/ARMCM0/Include/ARMCM0.h"
// #endif
// #if defined(ENABLE_FMRADIO)
// #include "app/fm.h"
// #endif
// #include "app/uart.h"
// #include "board.h"
// #include "bsp/dp32g030/dma.h"
// #include "bsp/dp32g030/gpio.h"
// #include "driver/aes.h"
// #include "driver/bk4819.h"
// #include "driver/crc.h"
// #include "driver/eeprom.h"
// #include "driver/gpio.h"
#include "driver/uart.h" // без этого не компилируется UART
// #include "functions.h"
// #include "misc.h"
// #include "settings.h"
// #if defined(ENABLE_OVERLAY)
// #include "sram-overlay.h"
// #endif
// #include "version.h"

// #ifdef ENABLE_UART_CAT
#include "../driver/system.h" // без этого не компилируется delay
// #include "external/printf/printf.h"
// #endif
//---------------------------------------------------------------

void HELLOWORLD_key(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld);
void HELLOWORLD_update(void);
void HELLOWORLD_render(void);

#endif /* end of include guard: SCANLIST_H */