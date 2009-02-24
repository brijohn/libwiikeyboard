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
#include <ogc/lwp_queue.h>
#include <wiikeyboard/keyboard.h>

#include "keymapper.h"

struct kbd_manager
{
	USBKeyboard kbd;
	struct symbol keymap[256];
	
	lwp_queue queue;
};

typedef struct _node
{
	lwp_node node;
	keyboard_event event;
} node;

static struct kbd_manager *_manager = NULL;

// keymaps from keymaps.c
extern struct keymap kbdmap_US;
extern struct keymap kbdmap_DE;

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

/* 
	Functions converts scancode + modifiers to keysym.
	Returns valid keysym or 0xfffe if no symbol is mapped
	to scancode.
*/
static u16 _kbd_mapSym(u8 scancode, u16 modifiers)
{
	struct symbol *ref = &_manager->keymap[scancode];
	u16 *group;
	u8 shift = ((modifiers & KMOD_LSHIFT) || (modifiers & KMOD_RSHIFT));

	if (ref->scancode != scancode)
		return 0xfffe;

	if (modifiers & KMOD_RALT)
		group = ref->group2;
	else
		group = ref->group1;

	if (!shift)
		return group[0];
	else
		return (group[1] == KBD_null ? group[0] : group[1]);
	
	return 0xfffe;
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
	event.keysym.sym = _kbd_mapSym(event.keysym.scancode, kbd->modifiers);
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

	if (event.keysym.sym == KBD_Numlock && event.type == KEYBOARD_PRESSED) {
		_kbd_setModifier(KMOD_NUM, !(kbd->modifiers & KMOD_NUM));
		USBKeyboard_ToggleLed(&_manager->kbd, USBKEYBOARD_LEDNUM);
	}

	if (event.keysym.sym == KBD_Capslock && event.type == KEYBOARD_PRESSED) {
		_kbd_setModifier(KMOD_CAPS, !(kbd->modifiers & KMOD_CAPS));
		USBKeyboard_ToggleLed(&_manager->kbd, USBKEYBOARD_LEDCAPS);
	}

	if (event.keysym.sym == KBD_Scrollock && event.type == KEYBOARD_PRESSED)
		USBKeyboard_ToggleLed(&_manager->kbd, USBKEYBOARD_LEDSCROLL);

	event.keysym.mod = kbd->modifiers;
	_kbd_addEvent(event);

	return 1;
}

//This function call usb function to check if a new keyboard is connected
static s32 _kbd_scan_for_keyboard(void)
{
	u16 vid, pid;
	s32 ret;

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

static void * _kbd_thread_func(void *arg) {
	u32 turns = 0;

	_kbd_scan_for_keyboard();

	while (!_kbd_thread_quit) {
		// scan for new attached keyboards
		turns++;
		if (turns % (KBD_THREAD_KBD_SCAN_INTERVAL) == 0) {
			if (!_manager->kbd.connect)
				_kbd_scan_for_keyboard();

			turns = 0;
		}

		USBKeyboard_Scan(&_manager->kbd);
		usleep(KBD_THREAD_UDELAY);
	}

	return NULL;
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

		keyboard_keymap map;

		switch (CONF_GetLanguage()) {
		case CONF_LANG_GERMAN:
			map = KEYBOARD_KEYMAP_DE;
			break;

		// TODO: integrate these keymaps in keymaps.c
		case CONF_LANG_JAPANESE:
		case CONF_LANG_FRENCH:
		case CONF_LANG_SPANISH:
		case CONF_LANG_ITALIAN:
		case CONF_LANG_DUTCH:
		case CONF_LANG_SIMP_CHINESE:
		case CONF_LANG_TRAD_CHINESE:
		case CONF_LANG_KOREAN:
		default:
			map = KEYBOARD_KEYMAP_US;
			break;
		}

		KEYBOARD_LoadKeyMap(map);
	}

	if (!_kbd_thread_running) {
		// start the keyboard thread
		_kbd_thread_quit = false;

		_kbd_stack = (u8 *) memalign(32, KBD_THREAD_STACKSIZE);

		if (!_kbd_stack)
			return -4;

		memset(_kbd_stack, 0, KBD_THREAD_STACKSIZE);

		LWP_InitQueue(&_kbd_queue);

		s32 res = LWP_CreateThread(&_kbd_thread, _kbd_thread_func, NULL,
									_kbd_stack, KBD_THREAD_STACKSIZE,
									KBD_THREAD_PRIO);

		if (res) {
			LWP_CloseQueue(_kbd_queue);
			free(_kbd_stack);

			USBKeyboard_Close(&_manager->kbd);
			free(_manager);

			return -5;
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

	if (_manager) {
		USBKeyboard_Close(&_manager->kbd);
		KEYBOARD_FlushEvents();
		free(_manager);
	}

	return 1;
}

s32 KEYBOARD_LoadKeyMap(const keyboard_keymap keymap) {
	struct keymap *map;
	u8 i;

	if (!_manager)
		return -1;

	switch (keymap) {
	case KEYBOARD_KEYMAP_DE:
		map = &kbdmap_DE;
		break;

	default:
		map = &kbdmap_US;
		break;
	}

	memset(_manager->keymap, 0, sizeof(struct symbol) * 256);

	for (i = 0; i < map->map_length; i++)
		_manager->keymap[map->symbols[i].scancode] = map->symbols[i];

	return 1;
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

