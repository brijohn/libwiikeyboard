#ifndef __KEYMAPPER_H__
#define __KEYMAPPER_H__

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

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
