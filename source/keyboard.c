/*-------------------------------------------------------------

keyboard.c -- keyboard event system

Copyright (C) 2008
DAVY Guillaume davyg2@gmail.com

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>

#include <gccore.h>
#include <ogc/usb.h>
#include <ogc/lwp_queue.h>

#include <keyboard.h>

#include "convertscan.h"

struct _keyManager
{
	device dev[DEVLIST_MAXSIZE];
	keyboard key[DEVLIST_MAXSIZE];
	u8 num;
	
	lwp_queue *queue;
};

typedef struct _node
{
	lwp_node node;
	keyboardEvent event;
}node;

static struct _keyManager _manager;

//Return the number of keyboard of device dev
s32 KEYBOARD_getNum(device dev)
{
	u8 i=0;
	for (i=0;i<_manager.num;i++)
		if (devEqual(dev,_manager.dev[i]))
			return i;
	return -1;
}

//Add an event to the event queue
s32 KEYBOARD_addEvent(keyboardEvent event)
{
	node *n = malloc(sizeof(node));
	n->event = event;
	__lwp_queue_append(_manager.queue,(lwp_node*)n);
	return 1;
}

//Event callback, call when an event occurs in usbkeyboard
s32 _event_cb(USBKeyboard_event kevent,void *usrdata)
{
	keyboard *key = (keyboard*) usrdata;
	keyboardEvent event;
	event.type = kevent.type;
	event.dev = key->dev;
	if (event.type==KEYBOARD_DISCONNECTED)
	{
		u8 j;
		for (j=KEYBOARD_getNum(key->dev)-1;j<_manager.num;j++)
		{
			_manager.dev[j] = _manager.dev[j+1];
			_manager.key[j] = _manager.key[j+1];
		}
		KEYBOARD_addEvent(event);
		return 1;
	}
	event.keysym.scancode = kevent.keyCode;
	event.keysym.mod = 
		KMOD_LCTRL	*((kevent.state>>0)&1) |
		KMOD_LSHIFT	*((kevent.state>>1)&1) |
		KMOD_LALT	*((kevent.state>>2)&1) |
		KMOD_LMETA	*((kevent.state>>3)&1) |
		KMOD_RCTRL	*((kevent.state>>4)&1) |
		KMOD_RSHIFT	*((kevent.state>>5)&1) |
		KMOD_RALT	*((kevent.state>>6)&1) |
		KMOD_RMETA	*((kevent.state>>7)&1) |
		KMOD_NUM	*(kevent.keyCode == 83) |
		KMOD_CAPS	*(kevent.keyCode == 57);
	event.keysym.sym = convertscan[kevent.keyCode];
	event.keysym.ch = event.keysym.sym;
	u8 i;
	if (event.keysym.sym == KEYBOARD_NUMLOCK && event.type == KEYBOARD_RELEASED)
		for (i=0;i<_manager.num;i++)
			USBKeyboard_SwitchLed(&_manager.key[i],USBKEYBOARD_LEDNUM);
	if (event.keysym.sym == KEYBOARD_CAPSLOCK && event.type == KEYBOARD_RELEASED)
		for (i=0;i<_manager.num;i++)
			USBKeyboard_SwitchLed(&_manager.key[i],USBKEYBOARD_LEDCAPS);
	if (event.keysym.sym == KEYBOARD_SCROLLOCK && event.type == KEYBOARD_RELEASED)
		for (i=0;i<_manager.num;i++)
			USBKeyboard_SwitchLed(&_manager.key[i],USBKEYBOARD_LEDSCROLL);
	KEYBOARD_addEvent(event);
	return 1;
}

//Thisfunction call usb function to check if a new keyboard is connected
s32 KEYBOARD_ScanForNewKeyboard()
{
	device dev[DEVLIST_MAXSIZE];
	
	s32 num;
	num = USBKeyboard_Find(&dev);
	if (num<0)
		return -1;
	if (num==0)
		return 0;
	
	u8 i;
	for (i=0;i<num;i++)
	{
		_manager.dev[_manager.num]=dev[i];
		s32 ret;
		ret = USBKeyboard_Open(&_manager.key[_manager.num],_manager.dev[_manager.num]);
		if (ret==1)
		{
			USBKeyboard_Add_EventCB(&_manager.key[_manager.num], &_event_cb,(void*) &_manager.key[_manager.num]);

			USBKeyboard_PutOnLed(&_manager.key[_manager.num],USBKEYBOARD_LEDNUM);
			USBKeyboard_PutOnLed(&_manager.key[_manager.num],USBKEYBOARD_LEDCAPS);
			USBKeyboard_PutOnLed(&_manager.key[_manager.num],USBKEYBOARD_LEDSCROLL);
			sleep(1);
			USBKeyboard_PutOffLed(&_manager.key[_manager.num],USBKEYBOARD_LEDNUM);
			USBKeyboard_PutOffLed(&_manager.key[_manager.num],USBKEYBOARD_LEDCAPS);
			USBKeyboard_PutOffLed(&_manager.key[_manager.num],USBKEYBOARD_LEDSCROLL);

			keyboardEvent event;
			event.type = KEYBOARD_CONNECTED;
			event.dev = _manager.dev[_manager.num];
			KEYBOARD_addEvent(event);

			_manager.num++;
		}
	}
	return _manager.num;
}

//Initialize USB and USB_KEYBOARD and the event queue
s32 KEYBOARD_Init()
{
	if (USB_Initialize()!=IPC_OK)
		return -1;

	if (USBKeyboard_Initialize()!=IPC_OK)
	{
		USB_Deinitialize();
		return -2;
	}
	_manager.queue = malloc(sizeof(lwp_queue));
	__lwp_queue_initialize(_manager.queue,0,0,0);
	_manager.num=0;
	s32 ret = KEYBOARD_ScanForNewKeyboard();
	return ret;
}

//Deinitialize USB and USB_KEYBOARD and the event queue
s32 KEYBOARD_Deinit()
{
	u8 i;
	for (i=0;i<_manager.num;i++)
		USBKeyboard_Close(&_manager.key[i]);
	USBKeyboard_Deinitialize();
	USB_Deinitialize();
	return 1;
}

//Get the first event of the event queue
s32 KEYBOARD_getEvent(keyboardEvent* event)
{
	node *n = (node*) __lwp_queue_get(_manager.queue);
	if (!n)
		return 0;
	*event = n->event;
	return 1;
}

//Function who call the check event for all keyboard
s32 KEYBOARD_ScanKeyboards()
{
	u8 i;
	for (i=0;i<_manager.num;i++)
	{
		USBKeyboard_GetState(&_manager.key[i]);
	}
	KEYBOARD_ScanForNewKeyboard();
	return 1;
}

//Put on the led l of keyboard dev
s32 KEYBOARD_putOnLed(keyboard_led l)
{
	return USBKeyboard_PutOnLed(&(_manager.key[0]),l);
}

//Put off the led l of keyboard dev
s32 KEYBOARD_putOffLed(keyboard_led l)
{
	return USBKeyboard_PutOffLed(&(_manager.key[0]),l);
}

//Switch led l of keyboard dev
s32 KEYBOARD_switchLed(keyboard_led l)
{
	return USBKeyboard_SwitchLed(&(_manager.key[0]),l);
}

//Look if a led is on or off
bool KEYBOARD_getLed(keyboard_led led)
{
	return ((_manager.key[0].leds & (1 << led ))>0);
}



