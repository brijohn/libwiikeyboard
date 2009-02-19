/*-------------------------------------------------------------

usbkeyboard.h -- Usb keyboard support(boot protocol)

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

#ifndef __USBKEYBOARD_H__
#define __USBKEYBOARD_H__

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

typedef enum
{
	USBKEYBOARD_PRESSED = 0,
	USBKEYBOARD_RELEASED,
	USBKEYBOARD_DISCONNECTED
} usb_keyboard_event_type;

typedef enum
{
	USBKEYBOARD_LEDNUM = 0,
	USBKEYBOARD_LEDCAPS,
	USBKEYBOARD_LEDSCROLL
} usb_keyboard_led;

typedef struct
{
	usb_keyboard_event_type type;
	u8 keyCode;
} usb_keyboard_event;

typedef void (*eventcallback) (usb_keyboard_event event);

s32 _usb_keyboard_init(void);
s32 _usb_keyboard_deinit(void);

s32 _usb_keyboard_open(const eventcallback cb);
void _usb_keyboard_close(void);

bool _usb_keyboard_is_connected(void);
s32 _usb_keyboard_scan(void);

s32 _usb_keyboard_set_led(const usb_keyboard_led led, bool on);
s32 _usb_keyboard_toggle_led(const usb_keyboard_led led);
bool _usb_keyboard_get_led(const usb_keyboard_led led);

#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif /* __USBKEYBOARD_H__ */

