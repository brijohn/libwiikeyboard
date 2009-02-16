/*-------------------------------------------------------------
keyboard_keysym.h

Copyright (C) 2008, 2009
DAVY Guillaume davyg2@gmail.com
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


#ifndef _KEYBOARD_KEYSYM_H
#define _KEYBOARD_KEYSYM_H

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

#define KBD_null                          0xffff

#define KBD_backspace                     0x0008
#define KBD_tab                           0x0009
#define KBD_clear                         0x000c
#define KBD_return                        0x000a
#define KBD_pause                         0x0013
#define KBD_escape                        0x001b

#define KBD_space                         0x0020
#define KBD_exclam                        0x0021
#define KBD_quotedbl                      0x0022
#define KBD_numbersign                    0x0023
#define KBD_dollar                        0x0024
#define KBD_percent                       0x0025
#define KBD_ampersand                     0x0026
#define KBD_apostrophe                    0x0027
#define KBD_parenleft                     0x0028
#define KBD_parenright                    0x0029
#define KBD_asterisk                      0x002a
#define KBD_plus                          0x002b
#define KBD_comma                         0x002c
#define KBD_minus                         0x002d
#define KBD_period                        0x002e
#define KBD_slash                         0x002f
#define KBD_0                             0x0030
#define KBD_1                             0x0031
#define KBD_2                             0x0032
#define KBD_3                             0x0033
#define KBD_4                             0x0034
#define KBD_5                             0x0035
#define KBD_6                             0x0036
#define KBD_7                             0x0037
#define KBD_8                             0x0038
#define KBD_9                             0x0039
#define KBD_colon                         0x003a
#define KBD_semicolon                     0x003b
#define KBD_less                          0x003c
#define KBD_equal                         0x003d
#define KBD_greater                       0x003e
#define KBD_question                      0x003f
#define KBD_at                            0x0040
#define KBD_A                             0x0041
#define KBD_B                             0x0042
#define KBD_C                             0x0043
#define KBD_D                             0x0044
#define KBD_E                             0x0045
#define KBD_F                             0x0046
#define KBD_G                             0x0047
#define KBD_H                             0x0048
#define KBD_I                             0x0049
#define KBD_J                             0x004a
#define KBD_K                             0x004b
#define KBD_L                             0x004c
#define KBD_M                             0x004d
#define KBD_N                             0x004e
#define KBD_O                             0x004f
#define KBD_P                             0x0050
#define KBD_Q                             0x0051
#define KBD_R                             0x0052
#define KBD_S                             0x0053
#define KBD_T                             0x0054
#define KBD_U                             0x0055
#define KBD_V                             0x0056
#define KBD_W                             0x0057
#define KBD_X                             0x0058
#define KBD_Y                             0x0059
#define KBD_Z                             0x005a
#define KBD_bracketleft                   0x005b
#define KBD_backslash                     0x005c
#define KBD_bracketright                  0x005d
#define KBD_asciicircum                   0x005e
#define KBD_underscore                    0x005f
#define KBD_grave                         0x0060
#define KBD_a                             0x0061
#define KBD_b                             0x0062
#define KBD_c                             0x0063
#define KBD_d                             0x0064
#define KBD_e                             0x0065
#define KBD_f                             0x0066
#define KBD_g                             0x0067
#define KBD_h                             0x0068
#define KBD_i                             0x0069
#define KBD_j                             0x006a
#define KBD_k                             0x006b
#define KBD_l                             0x006c
#define KBD_m                             0x006d
#define KBD_n                             0x006e
#define KBD_o                             0x006f
#define KBD_p                             0x0070
#define KBD_q                             0x0071
#define KBD_r                             0x0072
#define KBD_s                             0x0073
#define KBD_t                             0x0074
#define KBD_u                             0x0075
#define KBD_v                             0x0076
#define KBD_w                             0x0077
#define KBD_x                             0x0078
#define KBD_y                             0x0079
#define KBD_z                             0x007a
#define KBD_braceleft                     0x007b
#define KBD_bar                           0x007c
#define KBD_braceright                    0x007d
#define KBD_asciitilde                    0x007e

#define KBD_delete                        0x007f

#define KBD_nobreakspace                  0x00a0
#define KBD_exclamdown                    0x00a1
#define KBD_cent                          0x00a2
#define KBD_sterling                      0x00a3
#define KBD_currency                      0x00a4
#define KBD_yen                           0x00a5
#define KBD_brokenbar                     0x00a6
#define KBD_section                       0x00a7
#define KBD_diaeresis                     0x00a8
#define KBD_copyright                     0x00a9
#define KBD_ordfeminine                   0x00aa
#define KBD_guillemotleft                 0x00ab
#define KBD_notsign                       0x00ac
#define KBD_hyphen                        0x00ad
#define KBD_registered                    0x00ae
#define KBD_macron                        0x00af
#define KBD_degree                        0x00b0
#define KBD_plusminus                     0x00b1
#define KBD_twosuperior                   0x00b2
#define KBD_threesuperior                 0x00b3
#define KBD_acute                         0x00b4
#define KBD_mu                            0x00b5
#define KBD_paragraph                     0x00b6
#define KBD_periodcentered                0x00b7
#define KBD_cedilla                       0x00b8
#define KBD_onesuperior                   0x00b9
#define KBD_masculine                     0x00ba
#define KBD_guillemotright                0x00bb
#define KBD_onequarter                    0x00bc
#define KBD_onehalf                       0x00bd
#define KBD_threequarters                 0x00be
#define KBD_questiondown                  0x00bf
#define KBD_Agrave                        0x00c0
#define KBD_Aacute                        0x00c1
#define KBD_Acircumflex                   0x00c2
#define KBD_Atilde                        0x00c3
#define KBD_Adiaeresis                    0x00c4
#define KBD_Aring                         0x00c5
#define KBD_AE                            0x00c6
#define KBD_Ccedilla                      0x00c7
#define KBD_Egrave                        0x00c8
#define KBD_Eacute                        0x00c9
#define KBD_Ecircumflex                   0x00ca
#define KBD_Ediaeresis                    0x00cb
#define KBD_Igrave                        0x00cc
#define KBD_Iacute                        0x00cd
#define KBD_Icircumflex                   0x00ce
#define KBD_Idiaeresis                    0x00cf
#define KBD_ETH                           0x00d0
#define KBD_Ntilde                        0x00d1
#define KBD_Ograve                        0x00d2
#define KBD_Oacute                        0x00d3
#define KBD_Ocircumflex                   0x00d4
#define KBD_Otilde                        0x00d5
#define KBD_Odiaeresis                    0x00d6
#define KBD_multiply                      0x00d7
#define KBD_Oslash                        0x00d8
#define KBD_Ugrave                        0x00d9
#define KBD_Uacute                        0x00da
#define KBD_Ucircumflex                   0x00db
#define KBD_Udiaeresis                    0x00dc
#define KBD_Yacute                        0x00dd
#define KBD_THORN                         0x00de
#define KBD_ssharp                        0x00df
#define KBD_agrave                        0x00e0
#define KBD_aacute                        0x00e1
#define KBD_acircumflex                   0x00e2
#define KBD_atilde                        0x00e3
#define KBD_adiaeresis                    0x00e4
#define KBD_aring                         0x00e5
#define KBD_ae                            0x00e6
#define KBD_ccedilla                      0x00e7
#define KBD_egrave                        0x00e8
#define KBD_eacute                        0x00e9
#define KBD_ecircumflex                   0x00ea
#define KBD_ediaeresis                    0x00eb
#define KBD_igrave                        0x00ec
#define KBD_iacute                        0x00ed
#define KBD_icircumflex                   0x00ee
#define KBD_idiaeresis                    0x00ef
#define KBD_eth                           0x00f0
#define KBD_ntilde                        0x00f1
#define KBD_ograve                        0x00f2
#define KBD_oacute                        0x00f3
#define KBD_ocircumflex                   0x00f4
#define KBD_otilde                        0x00f5
#define KBD_odiaeresis                    0x00f6
#define KBD_division                      0x00f7
#define KBD_oslash                        0x00f8
#define KBD_ugrave                        0x00f9
#define KBD_uacute                        0x00fa
#define KBD_ucircumflex                   0x00fb
#define KBD_udiaeresis                    0x00fc
#define KBD_yacute                        0x00fd
#define KBD_thorn                         0x00fe
#define KBD_ydiaeresis                    0x00ff

#define KBD_F1                            0xd801
#define KBD_F2                            0xd802
#define KBD_F3                            0xd803
#define KBD_F4                            0xd804
#define KBD_F5                            0xd805
#define KBD_F6                            0xd806
#define KBD_F7                            0xd807
#define KBD_F8                            0xd808
#define KBD_F9                            0xd809
#define KBD_F10                           0xd80a
#define KBD_F11                           0xd80b
#define KBD_F12                           0xd80c
#define KBD_F13                           0xd80d
#define KBD_F14                           0xd80e
#define KBD_F15                           0xd80f

#define KBD_KP_0                          0xd930
#define KBD_KP_1                          0xd931
#define KBD_KP_2                          0xd932
#define KBD_KP_3                          0xd933
#define KBD_KP_4                          0xd934
#define KBD_KP_5                          0xd935
#define KBD_KP_6                          0xd936
#define KBD_KP_7                          0xd937
#define KBD_KP_8                          0xd938
#define KBD_KP_9                          0xd939
#define KBD_KP_asterisk                   0xd92a
#define KBD_KP_period                     0xd92e
#define KBD_KP_plus                       0xd92b
#define KBD_KP_minus                      0xd92d
#define KBD_KP_slash                      0xd92f
#define KBD_KP_equal                      0xd93d
#define KBD_KP_enter                      0xd90a

#define KBD_Up                            0xda00
#define KBD_Down                          0xda01
#define KBD_Right                         0xda02
#define KBD_Left                          0xda03
#define KBD_Home                          0xda04
#define KBD_End                           0xda05
#define KBD_Pageup                        0xda06
#define KBD_Pagedown                      0xda07

#define KBD_Insert                        0xdb00
#define KBD_Capslock                      0xdb01
#define KBD_Numlock                       0xdb02
#define KBD_Scrollock                     0xdb03
#define KBD_Help                          0xdb04
#define KBD_Print                         0xdb05
#define KBD_Sysreg                        0xdb06
#define KBD_Break                         0xdb07
#define KBD_Menu                          0xdb08
#define KBD_Power                         0xdb09
#define KBD_Euro                          0xdb0a
#define KBD_Undo                          0xdb0b
#define KBD_Application                   0xdb0c
#define KBD_Execute                       0xdb0d
#define KBD_Select                        0xdb0e
#define KBD_Stop                          0xdb0f
#define KBD_Again                         0xdb10
#define KBD_Cut                           0xdb11
#define KBD_Copy                          0xdb12
#define KBD_Paste                         0xdb13
#define KBD_Find                          0xdb14
#define KBD_Mute                          0xdb15
#define KBD_Volumeup                      0xdb16
#define KBD_Volumedown                    0xdb17

#define KBD_LeftShift                     0xdd00
#define KBD_RightShift                    0xdd01
#define KBD_LeftCtrl                      0xdd02
#define KBD_RightCtrl                     0xdd03
#define KBD_LeftAlt                       0xdd04
#define KBD_RightAlt                      0xdd05
#define KBD_LeftMeta                      0xdd06
#define KBD_RightMeta                     0xdd07

typedef enum {
	KMOD_NONE  = 0x0000,
	KMOD_LSHIFT= 0x0001,
	KMOD_RSHIFT= 0x0002,
	KMOD_LCTRL = 0x0040,
	KMOD_RCTRL = 0x0080,
	KMOD_LALT  = 0x0100,
	KMOD_RALT  = 0x0200,
	KMOD_LMETA = 0x0400,
	KMOD_RMETA = 0x0800,
	KMOD_NUM   = 0x1000,
	KMOD_CAPS  = 0x2000,
	KMOD_MODE  = 0x4000,
	KMOD_RESERVED = 0x8000
} KEYBOARD_MOD;


#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif /* _KEYBOARD_KEYSYM_H */