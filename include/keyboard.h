#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

#ifdef LIBBUILD
#include "usbkeyboard.h"
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

}keyboard_eventType;

typedef struct _keyboard_keysym
{  
	u32 scancode;
	KEYBOARD_KEY sym;
	char ch;
	KEYBOARD_MOD mod;

}keyboard_keysym;

typedef struct _KeyboardEvent{
	keyboard_eventType type;
	keyboard_keysym keysym;
	device dev;
}keyboardEvent;

typedef enum
{
	KEYBOARD_LEDNUM=0,
	KEYBOARD_LEDCAPS,
	KEYBOARD_LEDSCROLL
}keyboard_led;

s32 KEYBOARD_Init();
s32 KEYBOARD_Deinit();

s32 KEYBOARD_getEvent(keyboardEvent* event);

s32 KEYBOARD_ScanKeyboards();

s32 KEYBOARD_putOnLed(keyboard_led);
s32 KEYBOARD_putOffLed(keyboard_led);
s32 KEYBOARD_switchLed(keyboard_led);
bool KEYBOARD_getLed(keyboard_led);

#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif /* __KEYBOARD_H__ */

