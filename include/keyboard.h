/*-------------------------------------------------------------

keyboard.h -- keyboard event system

Copyright (C) 2008, 2009
DAVY Guillaume davyg2@gmail.com
Brian Johnson brijohn@gmail.com
dhewg

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.

2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source
distribution.

-------------------------------------------------------------*/

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

#ifdef LIBBUILD
#include "usbkeyboard.h"
#include "keymapper.h"
#else
#include <libwiikeyboard/usbkeyboard.h>
#endif

#include "keyboard_keysym.h"


typedef enum
{
	KEYBOARD_PRESSED = 0,
	KEYBOARD_RELEASED,
	KEYBOARD_DISCONNECTED,
	KEYBOARD_CONNECTED

} keyboard_event_type;

typedef struct _keyboard_keysym
{
	u8 scancode;
	u16 sym;
	KEYBOARD_MOD mod;

} keyboard_keysym;

typedef struct _KeyboardEvent {
	keyboard_event_type type;
	keyboard_keysym keysym;
} keyboard_event;

typedef enum
{
	KEYBOARD_LEDNUM=0,
	KEYBOARD_LEDCAPS,
	KEYBOARD_LEDSCROLL
} keyboard_led;

s32 KEYBOARD_Init(void);
s32 KEYBOARD_Deinit(void);

s32 KEYBOARD_ScanForKeyboard(void);

s32 KEYBOARD_InitKeyMap();
s32 KEYBOARD_LoadKeyMap(char* name);
u16 KEYBOARD_GetKeySym(u8 scancode, u16 modifiers);

s32 KEYBOARD_Scan(void);
s32 KEYBOARD_GetEvent(keyboard_event *event);
s32 KEYBOARD_FlushEvents(void);

s32 KEYBOARD_SetLed(const keyboard_led, bool on);
s32 KEYBOARD_ToggleLed(const keyboard_led);
bool KEYBOARD_GetLed(const keyboard_led);

#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif /* __KEYBOARD_H__ */

