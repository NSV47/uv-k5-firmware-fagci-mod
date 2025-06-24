#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include "../driver/keyboard.h"
#include "../driver/st7565.h"
#include "../external/printf/printf.h"
#include "../ui/helper.h"
#include <string.h>

void HELLOWORLD_key(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld);
void HELLOWORLD_update(void);
void HELLOWORLD_render(void);

#endif /* end of include guard: SCANLIST_H */