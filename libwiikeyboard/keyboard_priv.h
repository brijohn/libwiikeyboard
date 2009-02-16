#ifndef _KEYBOARD_PRIV_H
#define _KEYBOARD_PRIV_H

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

#include <ogc/lwp_queue.h>

struct kbd_manager
{
	USBKeyboard kbd;
	struct symbol keymap[256];
	
	lwp_queue queue;
};

extern struct kbd_manager *_manager;

#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif /* _KEYBOARD_PRIV_H */
