#ifndef _KEYBOARD_PRIV_H
#define _KEYBOARD_PRIV_H

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

#include <ogc/lwp_queue.h>

struct _keyManager
{
	device dev[DEVLIST_MAXSIZE];
	keyboard key[DEVLIST_MAXSIZE];
	struct symbol keymap[256];
	u8 num;
	
	lwp_queue *queue;
};

extern struct _keyManager _manager;

#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif /* _KEYBOARD_PRIV_H */
