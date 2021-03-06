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

#include <sys/types.h>
#include <sys/stat.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <gccore.h>
#include <ogc/usb.h>
#include <ogc/lwp_queue.h>
#include <ogc/lwp_watchdog.h>

#include "usbkeyboard.h"
#include <wiikeyboard/keyboard.h>

#include "wsksymvar.h"

keysym_t ksym_upcase(keysym_t);

extern const struct wscons_keydesc ukbd_keydesctab[];

static struct wskbd_mapdata _ukbd_keymapdata = {
	ukbd_keydesctab,
	KB_NONE
};

struct nameint {
	int value;
	char *name;
};

static struct nameint kbdenc_tab[] = {
	KB_ENCTAB
};

static struct nameint kbdvar_tab[] = {
	KB_VARTAB
};

static int _sc_maplen = 0;					/* number of entries in sc_map */
static struct wscons_keymap *_sc_map = 0;	/* current translation map */

static u16 _modifiers;

static int _composelen;		/* remaining entries in _composebuf */
static keysym_t _composebuf[2];

typedef struct {
	u8 keycode;
	u16 symbol;
} _keyheld;


typedef struct {
	bool enable;
	u8 keycode;
	u64 time;
	u8  initial_delay;
	u8  delay;
} _key_repeat;

static _key_repeat _repeat;

#define MAXHELD 8
static _keyheld _held[MAXHELD];
	
static lwp_queue _queue;

typedef struct {
	lwp_node node;
	keyboard_event event;
} _node;

static kbd_t _get_keymap_by_name(const char *identifier) {
	char name[64];
	u8 i, j;
	kbd_t encoding, variant;

	kbd_t res = KB_NONE;

	if (!identifier || (strlen(identifier) < 2))
		return res;

	i = 0;
	for (i = 0; ukbd_keydesctab[i].name != 0; ++i) {
		if (ukbd_keydesctab[i].name & KB_HANDLEDBYWSKBD)
			continue;

		encoding = KB_ENCODING(ukbd_keydesctab[i].name);
		variant = KB_VARIANT(ukbd_keydesctab[i].name);

		name[0] = 0;
		for (j = 0; j < sizeof(kbdenc_tab) / sizeof(struct nameint); ++j)
			if (encoding == kbdenc_tab[j].value) {
				strcpy(name, kbdenc_tab[j].name);
				break;
			}

		if (strlen(name) < 1)
			continue;

		for (j = 0; j < sizeof(kbdvar_tab) / sizeof(struct nameint); ++j)
			if (variant & kbdvar_tab[j].value) {
				strcat(name, "-");
				strcat(name,  kbdvar_tab[j].name);
			}

		if (!strcmp(identifier, name)) {
			res = ukbd_keydesctab[i].name;
			break;
		}
	}

	return res;
}

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
static void _kbd_event_cb(usb_keyboard_event kevent)
{
	keyboard_event event;
	keysym_t ksym;
	int i;

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

	ksym = KEYBOARD_KeycodeToKeysym(event.keycode, _modifiers);

	if (ksym == KS_voidSymbol)
		return;

	/* Now update modifiers */
	switch (ksym) {
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
		_usb_keyboard_set_led(USBKEYBOARD_LEDCAPS,
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
		_usb_keyboard_set_led(USBKEYBOARD_LEDNUM,
							MOD_ONESET(_modifiers, MOD_NUMLOCK));
		break;

	case KS_Hold_Screen:
		update_modifier(event.type, 1, MOD_HOLDSCREEN);
		_usb_keyboard_set_led(USBKEYBOARD_LEDSCROLL,
							MOD_ONESET(_modifiers, MOD_HOLDSCREEN));
		break;
	}

	/* Process compose sequence and dead accents */
	switch (KS_GROUP(ksym)) {
	case KS_GROUP_Mod:
		if (ksym == KS_Multi_key) {
			update_modifier(KEYBOARD_PRESSED, 0, MOD_COMPOSE);
			_composelen = 2;
		}
		break;

	case KS_GROUP_Dead:
		if (event.type != KEYBOARD_PRESSED)
			return;

		if (_composelen == 0) {
			update_modifier(KEYBOARD_PRESSED, 0, MOD_COMPOSE);
			_composelen = 1;
			_composebuf[0] = ksym;

			return;
		}
		break;
	}

	if ((event.type == KEYBOARD_PRESSED) && (_composelen > 0)) {
		if (KS_GROUP(ksym) != KS_GROUP_Mod) {
			_composebuf[2 - _composelen] = ksym;
			if (--_composelen == 0) {
				ksym = wskbd_compose_value(_composebuf);
				update_modifier(KEYBOARD_RELEASED, 0, MOD_COMPOSE);
			} else {
				return;
			}
		}
	}

	// store up to MAXHELD pressed events to match the symbol for release
	switch (KS_GROUP(ksym)) {
	case KS_GROUP_Ascii:
	case KS_GROUP_Keypad:
	case KS_GROUP_Function:
		if (event.type == KEYBOARD_PRESSED) {
			for (i = 0; i < MAXHELD; ++i) {
				if (_held[i].keycode == 0) {
					_held[i].keycode = event.keycode;
					_held[i].symbol = ksym;

					break;
				}
			}
		} else {
			for (i = 0; i < MAXHELD; ++i) {
				if (_held[i].keycode == event.keycode) {
					ksym = _held[i].symbol;
					_held[i].keycode = 0;
					_held[i].symbol = 0;

					break;
				}
			}
		}

		break;
	}

	event.symbol = ksym;
	event.modifiers = _modifiers;

	if (_repeat.enable) {
		if (event.type == KEYBOARD_PRESSED &&
		    (KS_GROUP(event.symbol) == KS_GROUP_Ascii ||
		     KS_GROUP(event.symbol) == KS_GROUP_Keypad ||
		     KS_GROUP(event.symbol) == KS_GROUP_Function)) {
			_repeat.keycode = event.keycode;
			_repeat.time = ticks_to_millisecs(gettime()) + _repeat.initial_delay;
		} else if (event.type == KEYBOARD_RELEASED) {
			if (event.keycode == _repeat.keycode) {
				_repeat.keycode = 0;
			}
		}
	}

	_kbd_addEvent(&event);

	return;
}

//This function call usb function to check if a new keyboard is connected
static s32 _kbd_scan_for_keyboard(void)
{
	s32 ret;
	keyboard_event event;

	ret = _usb_keyboard_open(&_kbd_event_cb);

	if (ret < 0)
		return ret;

	_modifiers = 0;
	_composelen = 0;
	memset(_held, 0, sizeof(_held));

	_usb_keyboard_set_led(USBKEYBOARD_LEDNUM, true);
	_usb_keyboard_set_led(USBKEYBOARD_LEDCAPS, true);
	_usb_keyboard_set_led(USBKEYBOARD_LEDSCROLL, true);
	usleep(200 * 1000);
	_usb_keyboard_set_led(USBKEYBOARD_LEDNUM, false);
	_usb_keyboard_set_led(USBKEYBOARD_LEDCAPS, false);
	_usb_keyboard_set_led(USBKEYBOARD_LEDSCROLL, false);

	event.type = KEYBOARD_CONNECTED;
	event.modifiers = 0;
	event.keycode = 0;

	_kbd_addEvent(&event);

	return ret;
}

static void * _kbd_thread_func(void *arg) {
	u32 turns = 0;

	while (!_kbd_thread_quit) {
		// scan for new attached keyboards
		if ((turns % KBD_THREAD_KBD_SCAN_INTERVAL) == 0) {
			if (!_usb_keyboard_is_connected())
				_kbd_scan_for_keyboard();

			turns = 0;
		}
		turns++;
		usleep(KBD_THREAD_UDELAY);
	}

	return NULL;
}

s32 KEYBOARD_LoadKeymap(char *name)
{
	kbd_t layout = _get_keymap_by_name(name);

	if (layout == KB_NONE)
		return -1;

	_ukbd_keymapdata.layout = layout;
	if (wskbd_load_keymap(&_ukbd_keymapdata, &_sc_map, &_sc_maplen) < 0) {
		_ukbd_keymapdata.layout = KB_NONE;
		return -4;
	}
	return 0;
}

s32 _init_default_keymap(void)
{
	int fd;
	struct stat st;
	char keymap[64];
	size_t i;
	s32 ret = -1;

	if (_ukbd_keymapdata.layout == KB_NONE) {
		keymap[0] = 0;
		fd = open("/wiikbd.map", O_RDONLY);

		if ((fd > 0) && !fstat(fd, &st)) {
			if ((st.st_size > 0) && (st.st_size < 64) &&
				(st.st_size == read(fd, keymap, st.st_size))) {
				keymap[63] = 0;
				for (i = 0; i < 64; ++i) {
					if ((keymap[i] != '-') && (isalpha(keymap[i]) == 0)) {
						keymap[i] = 0;
						break;
					}
				}
			}
			close(fd);
		}
		ret = KEYBOARD_LoadKeymap(keymap);
	}

	if (_ukbd_keymapdata.layout == KB_NONE) {
		switch (CONF_GetLanguage()) {
		case CONF_LANG_GERMAN:
			ret = KEYBOARD_LoadKeymap("de");
			break;

		case CONF_LANG_JAPANESE:
			ret = KEYBOARD_LoadKeymap("jp");
			break;

		case CONF_LANG_FRENCH:
			ret = KEYBOARD_LoadKeymap("fr");
			break;

		case CONF_LANG_SPANISH:
			ret = KEYBOARD_LoadKeymap("es");
			break;

		case CONF_LANG_ITALIAN:
			ret = KEYBOARD_LoadKeymap("it");
			break;

		case CONF_LANG_DUTCH:
			ret = KEYBOARD_LoadKeymap("nl");
			break;

		case CONF_LANG_SIMP_CHINESE:
		case CONF_LANG_TRAD_CHINESE:
		case CONF_LANG_KOREAN:
		default:
			ret = KEYBOARD_LoadKeymap("us");
			break;
		}
	}
	return ret;
}

//Initialize USB and USB_KEYBOARD and the event queue
s32 KEYBOARD_Init(void)
{

	if (USB_Initialize() != IPC_OK)
		return -1;

	if (_usb_keyboard_init() != IPC_OK) {
		USB_Deinitialize();
		return -2;
	}

	if (_init_default_keymap() < 0) {
		return -4;
	}

	__lwp_queue_initialize(&_queue, 0, 0, 0);

	KEYBOARD_SetKeyDelay(400, 100);
	KEYBOARD_EnableKeyRepeat(0);

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

			_usb_keyboard_close();

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

	_usb_keyboard_close();
	KEYBOARD_FlushEvents();
	_usb_keyboard_deinit();
	USB_Deinitialize();

	if (_sc_map) {
		free(_sc_map);
		_sc_map = NULL;
		_sc_maplen = 0;
	}

	return 1;
}

u16 KEYBOARD_KeycodeToKeysym(u8 keycode, u16 modifiers)
{
	struct wscons_keymap kp;
	int gindex;
	keysym_t *group;
	keysym_t ksym;

	if (keycode > _sc_maplen)
		return KS_voidSymbol;

	kp = _sc_map[keycode];

	/* Get the keysym */
	if (_modifiers & (MOD_MODESHIFT|MOD_MODELOCK) &&
	    !MOD_ONESET(_modifiers, MOD_ANYCONTROL))
		group = kp.group2;
	else
		group = kp.group1;

	if ((_modifiers & MOD_NUMLOCK) &&
	    KS_GROUP(group[1]) == KS_GROUP_Keypad) {
		gindex = !MOD_ONESET(_modifiers, MOD_ANYSHIFT);
		ksym = group[gindex];
	} else {
		/* CAPS alone should only affect letter keys */
		if ((_modifiers & (MOD_CAPSLOCK | MOD_ANYSHIFT)) ==
		    MOD_CAPSLOCK) {
			gindex = 0;
			ksym = ksym_upcase(group[0]);
		} else {
			gindex = MOD_ONESET(_modifiers, MOD_ANYSHIFT);
			ksym = group[gindex];
		}
	}

	return ksym;
}

//Get the first event of the event queue
s32 KEYBOARD_GetEvent(keyboard_event *event)
{
	_node *n = (_node *) __lwp_queue_get(&_queue);

	if (!n) {
		if (_repeat.enable) {
			s64 time = ticks_to_millisecs(gettime());
			if (_repeat.keycode != 0 &&
			    _repeat.time < time) {
				event->type = KEYBOARD_PRESSED;
				event->keycode = _repeat.keycode;
				event->modifiers = _modifiers;
				event->symbol =
					KEYBOARD_KeycodeToKeysym(_repeat.keycode, _modifiers);
				_repeat.time = time + _repeat.delay;
				return 1;
			}
		}
		return 0;
	}

	*event = n->event;

	free(n);

	return 1;
}

//Flush all pending events
s32 KEYBOARD_FlushEvents(void)
{
	s32 res;
	_node *n;

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

s32 KEYBOARD_EnableKeyRepeat(bool enable)
{
	_repeat.enable = enable;
	return 0;
}

s32 KEYBOARD_SetKeyDelay(u16 initial, u16 delay)
{
	_repeat.initial_delay = initial;
	_repeat.delay = delay;
	return 0;
}

//Turn on/off a led
s32 KEYBOARD_SetLed(const keyboard_led led, bool on)
{
	return _usb_keyboard_set_led(led, on);
}

//Toggle a led
s32 KEYBOARD_ToggleLed(const keyboard_led led)
{
	return _usb_keyboard_toggle_led(led);
}

//Check if a led is on or off
bool KEYBOARD_GetLed(const keyboard_led led)
{
	return _usb_keyboard_get_led(led);
}

