/*-------------------------------------------------------------

keymapper.c -- Keyboard layout system

Copyright (C) 2009
Brian Johnson brijohn@gmail.com

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
#include <sys/stat.h>
#include <gctypes.h>

#include <keyboard.h>
#include "keyboard_priv.h"

BEGIN_KEYMAP(default_map, 124)
	MAP(4, a, A)
	MAP(5, b, B)
	MAP(6, c, C)
	MAP(7, d, D)
	MAP(8, e, E)
	MAP(9, f, F)
	MAP(10, g, G)
	MAP(11, h, H)
	MAP(12, i, I)
	MAP(13, j, J)
	MAP(14, k, K)
	MAP(15, l, L)
	MAP(16, m, M)
	MAP(17, n, N)
	MAP(18, o, O)
	MAP(19, p, P)
	MAP(20, q, Q)
	MAP(21, r, R)
	MAP(22, s, S)
	MAP(23, t, T)
	MAP(24, u, U)
	MAP(25, v, V)
	MAP(26, w, W)
	MAP(27, x, X)
	MAP(28, y, Y)
	MAP(29, z, Z)
	MAP(30, 1, exclam)
	MAP(31, 2, at)
	MAP(32, 3, numbersign)
	MAP(33, 4, dollar)
	MAP(34, 5, percent)
	MAP(35, 6, asciicircum)
	MAP(36, 7, ampersand)
	MAP(37, 8, asterisk)
	MAP(38, 9, parenleft)
	MAP(39, 0, parenright)
	MAP(45, minus, underscore)
	MAP(46, plus, equal)
	MAP(47, bracketleft, braceleft)
	MAP(48, bracketright, braceright)
	MAP(49, backslash, bar)
	MAP(51, semicolon, colon)
	MAP(52, apostrophe, quotedbl)
	MAP(53, grave, asciitilde)
	MAP(54, comma, less)
	MAP(55, period, greater)
	MAP(56, slash, question)
	MAP(40, return, null)
	MAP(41, escape, null)
	MAP(42, backspace, null)
	MAP(43, tab, null)
	MAP(44, space, null)
	MAP(57, Capslock, null)
	MAP(58, F1, null)
	MAP(59, F2, null)
	MAP(60, F3, null)
	MAP(61, F4, null)
	MAP(62, F5, null)
	MAP(63, F6, null)
	MAP(64, F7, null)
	MAP(65, F8, null)
	MAP(66, F9, null)
	MAP(67, F10, null)
	MAP(68, F11, null)
	MAP(69, F12, null)
	MAP(70, Print, null)
	MAP(71, Scrollock, null)
	MAP(72, pause, null)
	MAP(73, Insert, null)
	MAP(74, Home, null)
	MAP(75, Pageup, null)
	MAP(76, delete, null)
	MAP(77, End, null)
	MAP(78, Pagedown, null)
	MAP(79, Right, null)
	MAP(80, Left, null)
	MAP(81, Down, null)
	MAP(82, Up, null)
	MAP(83, Numlock, null)
	MAP(84, KP_slash, null)
	MAP(85, KP_asterisk, null)
	MAP(86, KP_minus, null)
	MAP(87, KP_plus, null)
	MAP(88, KP_enter, null)
	MAP(89, KP_1, null)
	MAP(90, KP_2, null)
	MAP(91, KP_3, null)
	MAP(92, KP_4, null)
	MAP(93, KP_5, null)
	MAP(94, KP_6, null)
	MAP(95, KP_7, null)
	MAP(96, KP_8, null)
	MAP(97, KP_9, null)
	MAP(98, KP_0, null)
	MAP(99, KP_period, null)
	MAP(101, Application, null)
	MAP(102, Power, null)
	MAP(103, KP_equal, null)
	MAP(104, F13, null)
	MAP(105, F14, null)
	MAP(106, F15, null)
	MAP(116, Execute, null)
	MAP(117, Help, null)
	MAP(118, Menu, null)
	MAP(119, Select, null)
	MAP(120, Stop, null)
	MAP(121, Again, null)
	MAP(122, Undo, null)
	MAP(123, Cut, null)
	MAP(124, Copy, null)
	MAP(125, Paste, null)
	MAP(126, Find, null)
	MAP(127, Mute, null)
	MAP(128, Volumeup, null)
	MAP(129, Volumedown, null)
	MAP(224, LeftCtrl, null)
	MAP(225, LeftShift, null)
	MAP(226, LeftAlt, null)
	MAP(227, LeftMeta, null)
	MAP(228, RightCtrl, null)
	MAP(229, RightShift, null)
	MAP(230, RightAlt, null)
	MAP(231, RightMeta, null)
END_KEYMAP

s32 KEYBOARD_LoadKeyMap(char* name) {
	int i;
	FILE *fp;
	char filename[1024];
	unsigned char *map_buf = NULL;
	struct keymap *map;
	struct stat st;
	memset(_manager.keymap, 0, sizeof(struct symbol) * 256);
	snprintf(filename, 1024, "sd:/keymaps/%s.map", name);
	if (strcmp(name, "default") == 0) {
		map = &default_map;
	} else {
		fp = fopen(filename, "r");
		if (fp == NULL) {
			map = &default_map;
			goto load_map;
		}
		if(fstat(fileno(fp), &st) < 0) {
			fclose(fp);
			map = &default_map;
			goto load_map;
		}
		map_buf = malloc(st.st_size);
		if (map_buf == NULL) {
			fclose(fp);
			map = &default_map;
			goto load_map;
		}
		fread(map_buf, st.st_size, 1, fp);
		if (ferror(fp)) {
			free(map_buf);
			fclose(fp);
			map = &default_map;
			goto load_map;
		}
		map = (struct keymap*)map_buf;
		fclose(fp);
	}

load_map:
	printf("Loading Keymap: %s\n", map->name);
	for (i = 0; i < map->map_length; i++) {
		_manager.keymap[map->symbols[i].scancode] = map->symbols[i];
	}
	if (map_buf != NULL)
		free(map_buf);

	return 0;
}

s32 KEYBOARD_InitKeyMap()
{
	FILE *fp;
	char map_name[50];
	fp = fopen("sd:/kbd.map", "r");
	if (fp == NULL) {
		return KEYBOARD_LoadKeyMap("default");
	}
	if(fgets(map_name, 50, fp) == NULL) {
		fclose(fp);
		return KEYBOARD_LoadKeyMap("default");
	}
	if (map_name[strlen(map_name)-1] == '\n')
		map_name[strlen(map_name)-1] = '\0';
	if (map_name[strlen(map_name)-1] == '\r')
		map_name[strlen(map_name)-1] = '\0';
	fclose(fp);
	return KEYBOARD_LoadKeyMap(map_name);
}

/* 
	Functions converts scancode + modifiers to keysym.
	Returns valid keysym or 0xfffe if no symbol is mapped
	to scancode.
*/
u16 KEYBOARD_GetKeySym(u8 scancode, u16 modifiers)
{
	struct symbol *ref = &_manager.keymap[scancode];
	u16 *group;
	u8 shift = ((modifiers & KMOD_LSHIFT) || (modifiers & KMOD_RSHIFT));
	if (ref->scancode != scancode)
		return 0xfffe;
	if (modifiers & KMOD_RALT)
		group = ref->group2;
	else
		group = ref->group1;
	if (!shift) {
		return group[0];
	} else {
		return (group[1] == KBD_null ? group[0] : group[1]);
	}
	
	return 0xfffe;
}
