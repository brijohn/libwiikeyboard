#ifndef _KEYBOARD_PRIV_H
#define _KEYBOARD_PRIV_H

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

#include <ogc/lwp_queue.h>

struct key_repeater {
	keyboard_event ev;
	bool enable;
	s64 repeat_time;
	u16 repeat_delay;
};

struct kbd_manager
{
	USBKeyboard kbd;
	struct symbol keymap[256];
	
	struct key_repeater repeat;

	lwp_queue queue;
};

extern struct kbd_manager *_manager;

#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif /* _KEYBOARD_PRIV_H */
