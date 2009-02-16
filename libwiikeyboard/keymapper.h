/*-------------------------------------------------------------

keymapper.h -- keyboard mappings

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

#ifndef __KEYMAPPER_H__
#define __KEYMAPPER_H__

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

struct symbol {
	u8 scancode;
	u16 group1[2];
	u16 group2[2];
};

struct keymap {
	char name[12];
	long map_length;
	struct symbol symbols[];
};

#define BEGIN_KEYMAP(title, size) \
  struct keymap title = { \
	.name = #title, \
	.map_length = size, \
	.symbols = {


#define END_KEYMAP \
	}, \
  };

#define MAP(scan, ks1, ks2) \
		{scan, {KBD_##ks1, KBD_##ks2}, {KBD_##ks1, KBD_##ks2}},

#define EXT_MAP(scan, ks1, ks2, ks3, ks4) \
		{scan, {KBD_##ks1, KBD_##ks2}, {KBD_##ks3, KBD_##ks4}},

#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif /* __KEYMAPPER_H__ */
