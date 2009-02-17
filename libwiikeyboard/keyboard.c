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
#include <stdio.h>
#include <unistd.h>

#include <gccore.h>
#include <ogc/usb.h>
#include <ogc/lwp_queue.h>

#include <wiikeyboard/usbkeyboard.h>
#include <wiikeyboard/keyboard.h>

#include "wsksymvar.h"

keysym_t ksym_upcase(keysym_t);

extern const struct wscons_keydesc ukbd_keydesctab[];

static struct wskbd_mapdata _ukbd_keymapdata = {
	ukbd_keydesctab,
	KB_NONE
};

static int _sc_maplen = 0;					/* number of entries in sc_map */
static struct wscons_keymap *_sc_map = 0;	/* current translation map */

static USBKeyboard *_kbd = NULL;
static u16 _modifiers;

static lwp_queue _queue;

typedef struct {
	lwp_node node;
	keyboard_event event;
} _node;

#define KBD_THREAD_STACKSIZE (1024 * 4)
#define KBD_THREAD_PRIO 64
#define KBD_THREAD_UDELAY (1000 * 10)
#define KBD_THREAD_KBD_SCAN_INTERVAL (3 * 100)

static lwpq_t _kbd_queue;
static lwp_t _kbd_thread;
static u8 *_kbd_stack;
static bool _kbd_thread_running = false;
static bool _kbd_thread_quit = false;

//Add an event to the event queue
static s32 _kbd_addEvent(const keyboard_event *event) {
	_node *n = malloc(sizeof(_node));
	n->event = *event;

	__lwp_queue_append(&_queue, (lwp_node*) n);

	return 1;
}

void update_modifier(u_int type, int toggle, int mask) {
	if (toggle) {
		if (type == KEYBOARD_PRESSED)
			_modifiers ^= mask;
	} else {
		if (type == KEYBOARD_RELEASED)
			_modifiers &= ~mask;
		else
			_modifiers |= mask;
	}
}

//Event callback, gets called when an event occurs in usbkeyboard
static void _kbd_event_cb(USBKeyboard_event kevent, void *usrdata)
{
	keyboard_event event;
	struct wscons_keymap kp;
	keysym_t *group;
	int gindex;

	switch (kevent.type) {
	case USBKEYBOARD_DISCONNECTED:
		event.type = KEYBOARD_DISCONNECTED;
		event.modifiers = 0;
		event.keycode = 0;
		event.symbol = 0;

		_kbd_addEvent(&event);

		return;

	case USBKEYBOARD_PRESSED:
		event.type = KEYBOARD_PRESSED;
		break;

	case USBKEYBOARD_RELEASED:
		event.type = KEYBOARD_RELEASED;
		break;

	default:
		return;
	}

	event.keycode = kevent.keyCode;
	event.modifiers = _modifiers;

	wskbd_get_mapentry(&_ukbd_keymapdata, event.keycode, &kp);

	switch (kp.group1[0]) {
	case KS_Shift_L:
		update_modifier(event.type, 0, MOD_SHIFT_L);
		break;

	case KS_Shift_R:
		update_modifier(event.type, 0, MOD_SHIFT_R);
		break;

	case KS_Shift_Lock:
		update_modifier(event.type, 1, MOD_SHIFTLOCK);
		break;

	case KS_Caps_Lock:
		update_modifier(event.type, 1, MOD_CAPSLOCK);
		USBKeyboard_SetLed(_kbd, USBKEYBOARD_LEDCAPS,
							MOD_ONESET(_modifiers, MOD_CAPSLOCK));
		break;

	case KS_Control_L:
		update_modifier(event.type, 0, MOD_CONTROL_L);
		break;

	case KS_Control_R:
		update_modifier(event.type, 0, MOD_CONTROL_R);
		break;

	case KS_Alt_L:
		update_modifier(event.type, 0, MOD_META_L);
		break;

	case KS_Alt_R:
		update_modifier(event.type, 0, MOD_META_R);
		break;

	case KS_Mode_switch:
		update_modifier(event.type, 0, MOD_MODESHIFT);
		break;

	case KS_Mode_Lock:
		update_modifier(event.type, 1, MOD_MODELOCK);
		break;

	case KS_Num_Lock:
		update_modifier(event.type, 1, MOD_NUMLOCK);
		USBKeyboard_SetLed(_kbd, USBKEYBOARD_LEDNUM,
							MOD_ONESET(_modifiers, MOD_NUMLOCK));
		break;

	case KS_Hold_Screen:
		update_modifier(event.type, 1, MOD_HOLDSCREEN);
		USBKeyboard_SetLed(_kbd, USBKEYBOARD_LEDSCROLL,
							MOD_ONESET(_modifiers, MOD_HOLDSCREEN));
		break;
	}

	/* Get the keysym */
	if (_modifiers & (MOD_MODESHIFT|MOD_MODELOCK) &&
	    !MOD_ONESET(_modifiers, MOD_ANYCONTROL))
		group = &kp.group2[0];
	else
		group = &kp.group1[0];

	if ((_modifiers & MOD_NUMLOCK) &&
	    KS_GROUP(group[1]) == KS_GROUP_Keypad) {
		gindex = !MOD_ONESET(_modifiers, MOD_ANYSHIFT);
		event.symbol = group[gindex];
	} else {
		/* CAPS alone should only affect letter keys */
		if ((_modifiers & (MOD_CAPSLOCK | MOD_ANYSHIFT)) ==
		    MOD_CAPSLOCK) {
			gindex = 0;
			event.symbol = ksym_upcase(group[0]);
		} else {
			gindex = MOD_ONESET(_modifiers, MOD_ANYSHIFT);
			event.symbol = group[gindex];
		}
	}

	//printf("cmd=%x %x,%x | %x,%x -> %x \n", kp.command, kp.group1[0], kp.group1[1], kp.group2[0], kp.group2[1], event.symbol);

	_kbd_addEvent(&event);

	return;
}

//This function call usb function to check if a new keyboard is connected
static s32 _kbd_scan_for_keyboard(void)
{
	u16 vid, pid;
	s32 ret;
	keyboard_event event;

	ret = USBKeyboard_Find(&vid, &pid);

	if (ret < 1)
		return ret;
	
	ret = USBKeyboard_Open(_kbd, vid, pid);

	if (ret < 0)
		return ret;

	_modifiers = 0;

	USBKeyboard_SetCB(_kbd, &_kbd_event_cb, NULL);

	USBKeyboard_SetLed(_kbd, USBKEYBOARD_LEDNUM, true);
	USBKeyboard_SetLed(_kbd, USBKEYBOARD_LEDCAPS, true);
	USBKeyboard_SetLed(_kbd, USBKEYBOARD_LEDSCROLL, true);
	usleep(200 * 1000);
	USBKeyboard_SetLed(_kbd, USBKEYBOARD_LEDNUM, false);
	USBKeyboard_SetLed(_kbd, USBKEYBOARD_LEDCAPS, false);
	USBKeyboard_SetLed(_kbd, USBKEYBOARD_LEDSCROLL, false);

	event.type = KEYBOARD_CONNECTED;
	event.modifiers = 0;
	event.keycode = 0;

	_kbd_addEvent(&event);

	return ret;
}

static void * _kbd_thread_func(void *arg) {
	u32 turns = 0;

	_kbd_scan_for_keyboard();

	while (!_kbd_thread_quit) {
		// scan for new attached keyboards
		turns++;
		if (turns % (KBD_THREAD_KBD_SCAN_INTERVAL) == 0) {
			if (!_kbd->connect)
				_kbd_scan_for_keyboard();

			turns = 0;
		}

		USBKeyboard_Scan(_kbd);
		usleep(KBD_THREAD_UDELAY);
	}

	return NULL;
}

//Initialize USB and USB_KEYBOARD and the event queue
s32 KEYBOARD_Init(void)
{
	if (USB_Initialize() != IPC_OK)
		return -1;

	if (USBKeyboard_Initialize() != IPC_OK) {
		USB_Deinitialize();
		return -2;
	}

	if (!_kbd) {
		_kbd = (USBKeyboard *) malloc(sizeof(USBKeyboard));

		if (!_kbd)
			return -3;

		memset(_kbd, 0, sizeof(USBKeyboard));

		if (_ukbd_keymapdata.layout == KB_NONE) {
			//TODO: get the country code from the hid descriptor instead
			switch (CONF_GetLanguage()) {
			case CONF_LANG_GERMAN:
				_ukbd_keymapdata.layout = KB_DE | KB_NODEAD;
				break;

			case CONF_LANG_JAPANESE:
				_ukbd_keymapdata.layout = KB_JP;
				break;

			case CONF_LANG_FRENCH:
				_ukbd_keymapdata.layout = KB_FR;
				break;

			case CONF_LANG_SPANISH:
				_ukbd_keymapdata.layout = KB_ES;
				break;

			case CONF_LANG_ITALIAN:
				_ukbd_keymapdata.layout = KB_IT;
				break;

			case CONF_LANG_DUTCH:
				_ukbd_keymapdata.layout = KB_NL | KB_NODEAD;
				break;

			case CONF_LANG_SIMP_CHINESE:
			case CONF_LANG_TRAD_CHINESE:
			case CONF_LANG_KOREAN:
			default:
				_ukbd_keymapdata.layout = KB_US;
				break;
			}

			if (wskbd_load_keymap(&_ukbd_keymapdata, &_sc_map, &_sc_maplen) < 0) {
				free(_kbd);
				_kbd = NULL;
				_ukbd_keymapdata.layout = KB_NONE;

				return -4;
			}
		}

		__lwp_queue_initialize(&_queue, 0, 0, 0);
	}

	if (!_kbd_thread_running) {
		// start the keyboard thread
		_kbd_thread_quit = false;

		_kbd_stack = (u8 *) memalign(32, KBD_THREAD_STACKSIZE);

		if (!_kbd_stack)
			return -5;

		memset(_kbd_stack, 0, KBD_THREAD_STACKSIZE);

		LWP_InitQueue(&_kbd_queue);

		s32 res = LWP_CreateThread(&_kbd_thread, _kbd_thread_func, NULL,
									_kbd_stack, KBD_THREAD_STACKSIZE,
									KBD_THREAD_PRIO);

		if (res) {
			LWP_CloseQueue(_kbd_queue);
			free(_kbd_stack);

			USBKeyboard_Close(_kbd);
			free(_kbd);
			_kbd = NULL;

			return -6;
		}

		_kbd_thread_running = res == 0;
	}

	return 0;
}

//Deinitialize USB and USB_KEYBOARD and the event queue
s32 KEYBOARD_Deinit(void)
{
	if (_kbd_thread_running) {
		_kbd_thread_quit = true;
		LWP_ThreadBroadcast(_kbd_queue);

		LWP_JoinThread(_kbd_thread, NULL);
		LWP_CloseQueue(_kbd_queue);

		free(_kbd_stack);
		_kbd_thread_running = false;
	}

	USBKeyboard_Deinitialize();
	USB_Deinitialize();

	if (_kbd) {
		USBKeyboard_Close(_kbd);
		KEYBOARD_FlushEvents();
		free(_kbd);
		_kbd = NULL;
	}

	if (_sc_map) {
		free(_sc_map);
		_sc_map = NULL;
		_sc_maplen = 0;
	}

	return 1;
}

//Get the first event of the event queue
s32 KEYBOARD_GetEvent(keyboard_event *event)
{
	if (!_kbd)
		return -1;

	_node *n = (_node *) __lwp_queue_get(&_queue);

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
	_node *n;

	if (!_kbd)
		return -1;

	res = 0;
	while (true) {
		n = (_node *) __lwp_queue_get(&_queue);

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
	if (!_kbd)
		return -1;

	return USBKeyboard_SetLed(_kbd, led, on);
}

//Toggle a led
s32 KEYBOARD_ToggleLed(const keyboard_led led)
{
	if (!_kbd)
		return -1;

	return USBKeyboard_ToggleLed(_kbd, led);
}

//Check if a led is on or off
bool KEYBOARD_GetLed(const keyboard_led led)
{
	if (!_kbd)
		return -1;

	return (_kbd->leds & (1 << led)) > 0;
}

