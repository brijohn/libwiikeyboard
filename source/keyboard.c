/*-------------------------------------------------------------

keyboard.c -- keyboard event system

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

#include <string.h>
#include <malloc.h>
#include <unistd.h>

#include <gccore.h>
#include <ogc/usb.h>

#include <keyboard.h>
#include "keyboard_priv.h"

typedef struct _node
{
	lwp_node node;
	keyboard_event event;
} node;

struct kbd_manager *_manager = NULL;

//Add an event to the event queue
static s32 _kbd_addEvent(const keyboard_event event)
{
	node *n = malloc(sizeof(node));
	n->event = event;
	__lwp_queue_append(&_manager->queue, (lwp_node*) n);
	return 1;
}

static void _kbd_setModifier(const KEYBOARD_MOD mod, const u8 on) {
	if (on)
		_manager->kbd.modifiers |= mod;
	else
		_manager->kbd.modifiers &= ~mod;
}

//Event callback, gets called when an event occurs in usbkeyboard
static s32 _kbd_event_cb(USBKeyboard_event kevent, void *usrdata)
{
	USBKeyboard *kbd = (USBKeyboard*) usrdata;
	keyboard_event event;
	event.type = kevent.type;

	if (event.type == KEYBOARD_DISCONNECTED)
	{
		_kbd_addEvent(event);
		return 1;
	}

	event.keysym.scancode = kevent.keyCode;
	event.keysym.sym = KEYBOARD_GetKeySym(event.keysym.scancode, kbd->modifiers);
	if (event.keysym.sym == 0xfffe)
		return 0;

	if (event.keysym.sym == KBD_LeftShift )
		_kbd_setModifier(KMOD_LSHIFT, event.type == KEYBOARD_PRESSED);
	if (event.keysym.sym == KBD_RightShift)
		_kbd_setModifier(KMOD_RSHIFT, event.type == KEYBOARD_PRESSED);
	if (event.keysym.sym == KBD_LeftCtrl)
		_kbd_setModifier(KMOD_LCTRL, event.type == KEYBOARD_PRESSED);
	if (event.keysym.sym == KBD_RightCtrl)
		_kbd_setModifier(KMOD_RCTRL, event.type == KEYBOARD_PRESSED);
	if (event.keysym.sym == KBD_LeftAlt)
		_kbd_setModifier(KMOD_LALT, event.type == KEYBOARD_PRESSED);
	if (event.keysym.sym == KBD_RightAlt)
		_kbd_setModifier(KMOD_RALT, event.type == KEYBOARD_PRESSED);
	if (event.keysym.sym == KBD_LeftMeta)
		_kbd_setModifier(KMOD_LMETA, event.type == KEYBOARD_PRESSED);
	if (event.keysym.sym == KBD_RightMeta)
		_kbd_setModifier(KMOD_RMETA, event.type == KEYBOARD_PRESSED);

	if (event.keysym.sym == KBD_Numlock && event.type == KEYBOARD_RELEASED) {
		_kbd_setModifier(KMOD_NUM, !(kbd->modifiers & KMOD_NUM));
		USBKeyboard_ToggleLed(&_manager->kbd, USBKEYBOARD_LEDNUM);
	}

	if (event.keysym.sym == KBD_Capslock && event.type == KEYBOARD_RELEASED) {
		_kbd_setModifier(KMOD_CAPS, !(kbd->modifiers & KMOD_CAPS));
		USBKeyboard_ToggleLed(&_manager->kbd, USBKEYBOARD_LEDCAPS);
	}

	if (event.keysym.sym == KBD_Scrollock && event.type == KEYBOARD_RELEASED) {
		USBKeyboard_ToggleLed(&_manager->kbd, USBKEYBOARD_LEDSCROLL);
	}
	event.keysym.mod = kbd->modifiers;
	_kbd_addEvent(event);
	return 1;
}

//Initialize USB and USB_KEYBOARD and the event queue
s32 KEYBOARD_Init(void)
{
	if (USB_Initialize()!=IPC_OK)
		return -1;

	if (USBKeyboard_Initialize()!=IPC_OK) {
		USB_Deinitialize();
		return -2;
	}

	if (!_manager) {
		_manager = (struct kbd_manager *) malloc(sizeof (struct kbd_manager));

		if (!_manager)
			return -3;
		memset(_manager, 0, sizeof (struct kbd_manager));
		__lwp_queue_initialize(&_manager->queue,0,0,0);
	}

	if(KEYBOARD_InitKeyMap() < 0) {
		USB_Deinitialize();
		return -3;
	}

	return KEYBOARD_ScanForKeyboard();
}

//Deinitialize USB and USB_KEYBOARD and the event queue
s32 KEYBOARD_Deinit(void)
{
	USBKeyboard_Deinitialize();
	USB_Deinitialize();

	if (_manager) {
		USBKeyboard_Close(&_manager->kbd);
		KEYBOARD_FlushEvents();
		free(_manager);
	}
	return 1;
}

//This function call usb function to check if a new keyboard is connected
s32 KEYBOARD_ScanForKeyboard(void)
{
	u16 vid, pid;
	s32 ret;

	if (!_manager)
		return -1;

	if (_manager->kbd.connect)
		return 0;

	ret = USBKeyboard_Find(&vid, &pid);

	if (ret < 1)
		return ret;
	ret = USBKeyboard_Open(&_manager->kbd, vid, pid);

	if (ret < 0)
		return ret;

	USBKeyboard_SetCB(&_manager->kbd, &_kbd_event_cb, &_manager->kbd);

	USBKeyboard_SetLed(&_manager->kbd, USBKEYBOARD_LEDNUM, true);
	USBKeyboard_SetLed(&_manager->kbd, USBKEYBOARD_LEDCAPS, true);
	USBKeyboard_SetLed(&_manager->kbd, USBKEYBOARD_LEDSCROLL, true);
	usleep(200 * 1000);
	USBKeyboard_SetLed(&_manager->kbd, USBKEYBOARD_LEDNUM, false);
	USBKeyboard_SetLed(&_manager->kbd, USBKEYBOARD_LEDCAPS, false);
	USBKeyboard_SetLed(&_manager->kbd, USBKEYBOARD_LEDSCROLL, false);

	keyboard_event event;
	event.type = KEYBOARD_CONNECTED;
	memset (&event.keysym, 0, sizeof (keyboard_keysym));
	_kbd_addEvent(event);

	return ret;
}

//check for pending events
s32 KEYBOARD_Scan(void)
{
	if (!_manager)
		return -1;
	KEYBOARD_ScanForKeyboard();
	return USBKeyboard_Scan(&_manager->kbd);
}

//Get the first event of the event queue
s32 KEYBOARD_GetEvent(keyboard_event *event)
{
	if (!_manager)
		return -1;
	node *n = (node*) __lwp_queue_get(&_manager->queue);

	if (!n)
		return 0;

	*event = n->event;

	free(n);

	return 1;
}

//Flush all pending events
s32 KEYBOARD_FlushEvents(void)
{
	s32 res;
	node *n;

	if (!_manager)
		return -1;

	res = 0;
	while (true) {
		n = (node *) __lwp_queue_get(&_manager->queue);

		if (!n)
			break;

		free(n);
		res++;
	}

	return res;
}

//Turn on/off a led
s32 KEYBOARD_SetLed(const keyboard_led led, bool on)
{
	if (!_manager)
		return -1;

	return USBKeyboard_SetLed(&_manager->kbd, led, on);
}

//Toggle a led
s32 KEYBOARD_ToggleLed(const keyboard_led led)
{
	if (!_manager)
		return -1;

	return USBKeyboard_ToggleLed(&_manager->kbd, led);
}

//Check if a led is on or off
bool KEYBOARD_GetLed(const keyboard_led led)
{
	if (!_manager)
		return -1;

	return (_manager->kbd.leds & (1 << led)) > 0;
}

