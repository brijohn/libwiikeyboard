/*-
 * Copyright (c) 1997 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Juergen Hannken-Illjes.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DEV_WSCONS_WSKSYMDEF_H_
#define _DEV_WSCONS_WSKSYMDEF_H_

/*
 * Keysymbols encoded as 16-bit Unicode. Special symbols
 * are encoded in the private area (0xe000 - 0xf8ff).
 * Currently only ISO Latin-1 subset is supported.
 *
 * This file is parsed from userland. Encode keysyms as:
 *
 *	#define KS_[^ \t]* 0x[0-9a-f]*
 *
 * and don't modify the border comments.
 */

/*BEGINKEYSYMDECL*/

/*
 * Group Ascii (ISO Latin1) character in low byte
 */

#define	KS_BackSpace 		0x08
#define	KS_Tab 			0x09
#define	KS_Linefeed 		0x0a
#define	KS_Clear 		0x0b
#define	KS_Return 		0x0d
#define	KS_Escape 		0x1b
#define	KS_space 		0x20
#define	KS_exclam 		0x21
#define	KS_quotedbl 		0x22
#define	KS_numbersign 		0x23
#define	KS_dollar 		0x24
#define	KS_percent 		0x25
#define	KS_ampersand 		0x26
#define	KS_apostrophe 		0x27
#define	KS_parenleft 		0x28
#define	KS_parenright 		0x29
#define	KS_asterisk 		0x2a
#define	KS_plus 		0x2b
#define	KS_comma 		0x2c
#define	KS_minus 		0x2d
#define	KS_period 		0x2e
#define	KS_slash 		0x2f
#define	KS_0 			0x30
#define	KS_1 			0x31
#define	KS_2 			0x32
#define	KS_3 			0x33
#define	KS_4 			0x34
#define	KS_5 			0x35
#define	KS_6 			0x36
#define	KS_7 			0x37
#define	KS_8 			0x38
#define	KS_9 			0x39
#define	KS_colon 		0x3a
#define	KS_semicolon 		0x3b
#define	KS_less 		0x3c
#define	KS_equal 		0x3d
#define	KS_greater 		0x3e
#define	KS_question 		0x3f
#define	KS_at 			0x40
#define	KS_A 			0x41
#define	KS_B 			0x42
#define	KS_C 			0x43
#define	KS_D 			0x44
#define	KS_E 			0x45
#define	KS_F 			0x46
#define	KS_G 			0x47
#define	KS_H 			0x48
#define	KS_I 			0x49
#define	KS_J 			0x4a
#define	KS_K 			0x4b
#define	KS_L 			0x4c
#define	KS_M 			0x4d
#define	KS_N 			0x4e
#define	KS_O 			0x4f
#define	KS_P 			0x50
#define	KS_Q 			0x51
#define	KS_R 			0x52
#define	KS_S 			0x53
#define	KS_T 			0x54
#define	KS_U 			0x55
#define	KS_V 			0x56
#define	KS_W 			0x57
#define	KS_X 			0x58
#define	KS_Y 			0x59
#define	KS_Z 			0x5a
#define	KS_bracketleft 		0x5b
#define	KS_backslash 		0x5c
#define	KS_bracketright 	0x5d
#define	KS_asciicircum 		0x5e
#define	KS_underscore 		0x5f
#define	KS_grave 		0x60
#define	KS_a 			0x61
#define	KS_b 			0x62
#define	KS_c 			0x63
#define	KS_d 			0x64
#define	KS_e 			0x65
#define	KS_f 			0x66
#define	KS_g 			0x67
#define	KS_h 			0x68
#define	KS_i 			0x69
#define	KS_j 			0x6a
#define	KS_k 			0x6b
#define	KS_l 			0x6c
#define	KS_m 			0x6d
#define	KS_n 			0x6e
#define	KS_o 			0x6f
#define	KS_p 			0x70
#define	KS_q 			0x71
#define	KS_r 			0x72
#define	KS_s 			0x73
#define	KS_t 			0x74
#define	KS_u 			0x75
#define	KS_v 			0x76
#define	KS_w 			0x77
#define	KS_x 			0x78
#define	KS_y 			0x79
#define	KS_z 			0x7a
#define	KS_braceleft 		0x7b
#define	KS_bar 			0x7c
#define	KS_braceright 		0x7d
#define	KS_asciitilde 		0x7e
#define	KS_Delete 		0x7f

#define	KS_nobreakspace 	0xa0
#define	KS_exclamdown 		0xa1
#define	KS_cent 		0xa2
#define	KS_sterling 		0xa3
#define	KS_currency 		0xa4
#define	KS_yen 			0xa5
#define	KS_brokenbar 		0xa6
#define	KS_section 		0xa7
#define	KS_diaeresis 		0xa8
#define	KS_copyright 		0xa9
#define	KS_ordfeminine 		0xaa
#define	KS_guillemotleft 	0xab
#define	KS_notsign 		0xac
#define	KS_hyphen 		0xad
#define	KS_registered 		0xae
#define	KS_macron 		0xaf
#define	KS_degree 		0xb0
#define	KS_plusminus 		0xb1
#define	KS_twosuperior 		0xb2
#define	KS_threesuperior 	0xb3
#define	KS_acute 		0xb4
#define	KS_mu 			0xb5
#define	KS_paragraph 		0xb6
#define	KS_periodcentered 	0xb7
#define	KS_cedilla 		0xb8
#define	KS_onesuperior 		0xb9
#define	KS_masculine 		0xba
#define	KS_guillemotright 	0xbb
#define	KS_onequarter 		0xbc
#define	KS_onehalf 		0xbd
#define	KS_threequarters 	0xbe
#define	KS_questiondown 	0xbf
#define	KS_Agrave 		0xc0
#define	KS_Aacute 		0xc1
#define	KS_Acircumflex 		0xc2
#define	KS_Atilde 		0xc3
#define	KS_Adiaeresis 		0xc4
#define	KS_Aring 		0xc5
#define	KS_AE 			0xc6
#define	KS_Ccedilla 		0xc7
#define	KS_Egrave 		0xc8
#define	KS_Eacute 		0xc9
#define	KS_Ecircumflex 		0xca
#define	KS_Ediaeresis 		0xcb
#define	KS_Igrave 		0xcc
#define	KS_Iacute 		0xcd
#define	KS_Icircumflex 		0xce
#define	KS_Idiaeresis 		0xcf
#define	KS_ETH 			0xd0
#define	KS_Ntilde 		0xd1
#define	KS_Ograve 		0xd2
#define	KS_Oacute 		0xd3
#define	KS_Ocircumflex 		0xd4
#define	KS_Otilde 		0xd5
#define	KS_Odiaeresis 		0xd6
#define	KS_multiply 		0xd7
#define	KS_Ooblique 		0xd8
#define	KS_Ugrave 		0xd9
#define	KS_Uacute 		0xda
#define	KS_Ucircumflex 		0xdb
#define	KS_Udiaeresis 		0xdc
#define	KS_Yacute 		0xdd
#define	KS_THORN 		0xde
#define	KS_ssharp 		0xdf
#define	KS_agrave 		0xe0
#define	KS_aacute 		0xe1
#define	KS_acircumflex 		0xe2
#define	KS_atilde 		0xe3
#define	KS_adiaeresis 		0xe4
#define	KS_aring 		0xe5
#define	KS_ae 			0xe6
#define	KS_ccedilla 		0xe7
#define	KS_egrave 		0xe8
#define	KS_eacute 		0xe9
#define	KS_ecircumflex 		0xea
#define	KS_ediaeresis 		0xeb
#define	KS_igrave 		0xec
#define	KS_iacute 		0xed
#define	KS_icircumflex 		0xee
#define	KS_idiaeresis 		0xef
#define	KS_eth 			0xf0
#define	KS_ntilde 		0xf1
#define	KS_ograve 		0xf2
#define	KS_oacute 		0xf3
#define	KS_ocircumflex 		0xf4
#define	KS_otilde 		0xf5
#define	KS_odiaeresis 		0xf6
#define	KS_division 		0xf7
#define	KS_oslash 		0xf8
#define	KS_ugrave 		0xf9
#define	KS_uacute 		0xfa
#define	KS_ucircumflex 		0xfb
#define	KS_udiaeresis 		0xfc
#define	KS_yacute 		0xfd
#define	KS_thorn 		0xfe
#define	KS_ydiaeresis 		0xff

/*
 * Group Dead (dead accents)
 */

#define	KS_dead_grave 		0x0300
#define	KS_dead_acute 		0x0301
#define	KS_dead_circumflex 	0x0302
#define	KS_dead_tilde 		0x0303
#define	KS_dead_diaeresis 	0x0308
#define	KS_dead_abovering 	0x030a
#define	KS_dead_cedilla 	0x0327

/*
 * Latin 2 Group
 */

#define KS_Aogonek                    0x0104
#define KS_breve                      0x02D8
#define KS_Lstroke                    0x0141
#define KS_Lcaron                     0x013D
#define KS_Sacute                     0x015A
#define KS_Scaron                     0x0160
#define KS_Scedilla                   0x015E
#define KS_Tcaron                     0x0164
#define KS_Zacute                     0x0179
#define KS_Zcaron                     0x017D
#define KS_Zabovedot                  0x017B
#define KS_aogonek                    0x0105
#define KS_ogonek                     0x02DB
#define KS_lstroke                    0x0142
#define KS_lcaron                     0x013E
#define KS_sacute                     0x015B
#define KS_caron                      0x02C7
#define KS_scaron                     0x0161
#define KS_scedilla                   0x015F
#define KS_tcaron                     0x0165
#define KS_zacute                     0x017A
#define KS_doubleacute                0x02DD
#define KS_zcaron                     0x017E
#define KS_zabovedot                  0x017C
#define KS_Racute                     0x0154
#define KS_Abreve                     0x0102
#define KS_Lacute                     0x0139
#define KS_Cacute                     0x0106
#define KS_Ccaron                     0x010C
#define KS_Eogonek                    0x0118
#define KS_Ecaron                     0x011A
#define KS_Dcaron                     0x010E
#define KS_Dstroke                    0x0110
#define KS_Nacute                     0x0143
#define KS_Ncaron                     0x0147
#define KS_Odoubleacute               0x0150
#define KS_Rcaron                     0x0158
#define KS_Uring                      0x016E
#define KS_Udoubleacute               0x0170
#define KS_Tcedilla                   0x0162
#define KS_racute                     0x0155
#define KS_abreve                     0x0103
#define KS_lacute                     0x013A
#define KS_cacute                     0x0107
#define KS_ccaron                     0x010D
#define KS_eogonek                    0x0119
#define KS_ecaron                     0x011B
#define KS_dcaron                     0x010F
#define KS_dstroke                    0x0111
#define KS_nacute                     0x0144
#define KS_ncaron                     0x0148
#define KS_odoubleacute               0x0151
#define KS_udoubleacute               0x0171
#define KS_rcaron                     0x0159
#define KS_uring                      0x016F
#define KS_tcedilla                   0x0163
#define KS_abovedot                   0x02D9

/*
 * Latin 3 Group
 */

#define KS_Hstroke                    0x0126
#define KS_Hcircumflex                0x0124
#define KS_Iabovedot                  0x0130
#define KS_Gbreve                     0x011E
#define KS_Jcircumflex                0x0134
#define KS_hstroke                    0x0127
#define KS_hcircumflex                0x0125
#define KS_idotless                   0x0131
#define KS_gbreve                     0x011F
#define KS_jcircumflex                0x0135
#define KS_Cabovedot                  0x010A
#define KS_Ccircumflex                0x0108
#define KS_Gabovedot                  0x0120
#define KS_Gcircumflex                0x011C
#define KS_Ubreve                     0x016C
#define KS_Scircumflex                0x015C
#define KS_cabovedot                  0x010B
#define KS_ccircumflex                0x0109
#define KS_gabovedot                  0x0121
#define KS_gcircumflex                0x011D
#define KS_ubreve                     0x016D
#define KS_scircumflex                0x015D

/*
 * Latin 4 Group
 */
#define KS_kra                        0x0138
#define KS_Rcedilla                   0x0156
#define KS_Itilde                     0x0128
#define KS_Lcedilla                   0x013B
#define KS_Emacron                    0x0112
#define KS_Gcedilla                   0x0122
#define KS_Tslash                     0x0166
#define KS_rcedilla                   0x0157
#define KS_itilde                     0x0129
#define KS_lcedilla                   0x013C
#define KS_emacron                    0x0113
#define KS_gcedilla                   0x0123
#define KS_tslash                     0x0167
#define KS_ENG                        0x014A
#define KS_eng                        0x014B
#define KS_Amacron                    0x0100
#define KS_Iogonek                    0x012E
#define KS_Eabovedot                  0x0116
#define KS_Imacron                    0x012A
#define KS_Ncedilla                   0x0145
#define KS_Omacron                    0x014C
#define KS_Kcedilla                   0x0136
#define KS_Uogonek                    0x0172
#define KS_Utilde                     0x0168
#define KS_Umacron                    0x016A
#define KS_amacron                    0x0101
#define KS_iogonek                    0x012F
#define KS_eabovedot                  0x0117
#define KS_imacron                    0x012B
#define KS_ncedilla                   0x0146
#define KS_omacron                    0x014D
#define KS_kcedilla                   0x0137
#define KS_uogonek                    0x0173
#define KS_utilde                     0x0169
#define KS_umacron                    0x016B

/*
 * Latin 8 Group
 */

#define KS_Babovedot                  0x1e02
#define KS_babovedot                  0x1e03
#define KS_Dabovedot                  0x1e0a
#define KS_Wgrave                     0x1E80
#define KS_Wacute                     0x1E82
#define KS_dabovedot                  0x1E0B
#define KS_Ygrave                     0x1EF2
#define KS_Fabovedot                  0x1E1E
#define KS_fabovedot                  0x1E1F
#define KS_Mabovedot                  0x1E40
#define KS_mabovedot                  0x1E41
#define KS_Pabovedot                  0x1E56
#define KS_wgrave                     0x1E81
#define KS_pabovedot                  0x1E57
#define KS_wacute                     0x1E83
#define KS_Sabovedot                  0x1E60
#define KS_ygrave                     0x1EF3
#define KS_Wdiaeresis                 0x1E84
#define KS_wdiaeresis                 0x1E85
#define KS_sabovedot                  0x1E61
#define KS_Wcircumflex                0x0174
#define KS_Tabovedot                  0x1E6A
#define KS_Ycircumflex                0x0176
#define KS_wcircumflex                0x0175
#define KS_tabovedot                  0x1E6B
#define KS_ycircumflex                0x0177
#define KS_OE                         0x0152
#define KS_oe                         0x0153
#define KS_Ydiaeresis                 0x0178
#define KS_leftdblquot                0x201c
#define KS_dbllow9quot                0x201e
/*
 * Cyrillic Group
 */

#define KS_Cyrillic_GHE_bar           0x0492
#define KS_Cyrillic_ghe_bar           0x0493
#define KS_Cyrillic_ZHE_descender     0x0496
#define KS_Cyrillic_zhe_descender     0x0497
#define KS_Cyrillic_KA_descender      0x049A
#define KS_Cyrillic_ka_descender      0x049B
#define KS_Cyrillic_KA_vertstroke     0x049C
#define KS_Cyrillic_ka_vertstroke     0x049D
#define KS_Cyrillic_EN_descender      0x04A2
#define KS_Cyrillic_en_descender      0x04A3
#define KS_Cyrillic_U_straight        0x04AE
#define KS_Cyrillic_u_straight        0x04AF
#define KS_Cyrillic_U_straight_bar    0x04B0
#define KS_Cyrillic_u_straight_bar    0x04B1
#define KS_Cyrillic_HA_descender      0x04B2
#define KS_Cyrillic_ha_descender      0x04B3
#define KS_Cyrillic_CHE_descender     0x04B6
#define KS_Cyrillic_che_descender     0x04B7
#define KS_Cyrillic_CHE_vertstroke    0x04B8
#define KS_Cyrillic_che_vertstroke    0x04B9
#define KS_Cyrillic_SHHA              0x04BA
#define KS_Cyrillic_shha              0x04BB
#define KS_Cyrillic_SCHWA             0x04D8
#define KS_Cyrillic_schwa             0x04D9
#define KS_Cyrillic_I_macron          0x04E2
#define KS_Cyrillic_i_macron          0x04E3
#define KS_Cyrillic_O_bar             0x04E8
#define KS_Cyrillic_o_bar             0x04E9
#define KS_Cyrillic_U_macron          0x04EE
#define KS_Cyrillic_u_macron          0x04EF
#define KS_Serbian_dje                0x0452
#define KS_Macedonia_gje              0x0453
#define KS_Cyrillic_io                0x0451
#define KS_Ukrainian_ie               0x0454
#define KS_Macedonia_dse              0x0455
#define KS_Ukrainian_i                0x0456
#define KS_Ukrainian_yi               0x0457
#define KS_Cyrillic_je                0x0458
#define KS_Cyrillic_lje               0x0459
#define KS_Cyrillic_nje               0x045A
#define KS_Serbian_tshe               0x045B
#define KS_Macedonia_kje              0x045C
#define KS_Ukrainian_ghe              0x0491
#define KS_Byelorussian_shortu        0x045E
#define KS_Cyrillic_dzhe              0x045F
#define KS_numerosign                 0x2116
#define KS_Serbian_DJE                0x0402
#define KS_Macedonia_GJE              0x0403
#define KS_Cyrillic_IO                0x0401
#define KS_Ukrainian_IE               0x0404
#define KS_Macedonia_DSE              0x0405
#define KS_Ukrainian_I                0x0406
#define KS_Ukrainian_YI               0x0407
#define KS_Cyrillic_JE                0x0408
#define KS_Cyrillic_LJE               0x0409
#define KS_Cyrillic_NJE               0x040A
#define KS_Serbian_TSHE               0x040B
#define KS_Macedonia_KJE              0x040C
#define KS_Ukrainian_GHE              0x0490
#define KS_Byelorussian_SHORTU        0x040E
#define KS_Cyrillic_DZHE              0x040F
#define KS_Cyrillic_yu                0x044E
#define KS_Cyrillic_a                 0x0430
#define KS_Cyrillic_be                0x0431
#define KS_Cyrillic_tse               0x0446
#define KS_Cyrillic_de                0x0434
#define KS_Cyrillic_ie                0x0435
#define KS_Cyrillic_ef                0x0444
#define KS_Cyrillic_ghe               0x0433
#define KS_Cyrillic_ha                0x0445
#define KS_Cyrillic_i                 0x0438
#define KS_Cyrillic_shorti            0x0439
#define KS_Cyrillic_ka                0x043A
#define KS_Cyrillic_el                0x043B
#define KS_Cyrillic_em                0x043C
#define KS_Cyrillic_en                0x043D
#define KS_Cyrillic_o                 0x043E
#define KS_Cyrillic_pe                0x043F
#define KS_Cyrillic_ya                0x044F
#define KS_Cyrillic_er                0x0440
#define KS_Cyrillic_es                0x0441
#define KS_Cyrillic_te                0x0442
#define KS_Cyrillic_u                 0x0443
#define KS_Cyrillic_zhe               0x0436
#define KS_Cyrillic_ve                0x0432
#define KS_Cyrillic_softsign          0x044C
#define KS_Cyrillic_yeru              0x044B
#define KS_Cyrillic_ze                0x0437
#define KS_Cyrillic_sha               0x0448
#define KS_Cyrillic_e                 0x044D
#define KS_Cyrillic_shcha             0x0449
#define KS_Cyrillic_che               0x0447
#define KS_Cyrillic_hardsign          0x044A
#define KS_Cyrillic_YU                0x042E
#define KS_Cyrillic_A                 0x0410
#define KS_Cyrillic_BE                0x0411
#define KS_Cyrillic_TSE               0x0426
#define KS_Cyrillic_DE                0x0414
#define KS_Cyrillic_IE                0x0415
#define KS_Cyrillic_EF                0x0424
#define KS_Cyrillic_GHE               0x0413
#define KS_Cyrillic_HA                0x0425
#define KS_Cyrillic_I                 0x0418
#define KS_Cyrillic_SHORTI            0x0419
#define KS_Cyrillic_KA                0x041A
#define KS_Cyrillic_EL                0x041B
#define KS_Cyrillic_EM                0x041C
#define KS_Cyrillic_EN                0x041D
#define KS_Cyrillic_O                 0x041E
#define KS_Cyrillic_PE                0x041F
#define KS_Cyrillic_YA                0x042F
#define KS_Cyrillic_ER                0x0420
#define KS_Cyrillic_ES                0x0421
#define KS_Cyrillic_TE                0x0422
#define KS_Cyrillic_U                 0x0423
#define KS_Cyrillic_ZHE               0x0416
#define KS_Cyrillic_VE                0x0412
#define KS_Cyrillic_SOFTSIGN          0x042C
#define KS_Cyrillic_YERU              0x042B
#define KS_Cyrillic_ZE                0x0417
#define KS_Cyrillic_SHA               0x0428
#define KS_Cyrillic_E                 0x042D
#define KS_Cyrillic_SHCHA             0x0429
#define KS_Cyrillic_CHE               0x0427
#define KS_Cyrillic_HARDSIGN          0x042A

/*
 * Group 1 (modifiers)
 */

#define	KS_Shift_L 		0xf101
#define	KS_Shift_R 		0xf102
#define	KS_Control_L 		0xf103
#define	KS_Control_R 		0xf104
#define	KS_Caps_Lock 		0xf105
#define	KS_Shift_Lock 		0xf106
#define	KS_Alt_L 		0xf107
#define	KS_Alt_R 		0xf108
#define	KS_Multi_key 		0xf109
#define	KS_Mode_switch 		0xf10a
#define	KS_Num_Lock 		0xf10b
#define KS_Hold_Screen		0xf10c
#define KS_Cmd			0xf10d
#define KS_Cmd1			0xf10e
#define KS_Cmd2			0xf10f
#define KS_Meta_L		0xf110
#define KS_Meta_R		0xf111
#define KS_Zenkaku_Hankaku	0xf112	/* Zenkaku/Hankaku toggle */
#define KS_Hiragana_Katakana	0xf113	/* Hiragana/Katakana toggle */
#define KS_Henkan_Mode		0xf114	/* Start/Stop Conversion */
#define KS_Henkan		0xf115	/* Alias for Henkan_Mode */
#define KS_Muhenkan		0xf116	/* Cancel Conversion */
#define KS_Mode_Lock		0xf117

/*
 * Group 2 (keypad) character in low byte
 */

#define	KS_KP_F1 		0xf291
#define	KS_KP_F2 		0xf292
#define	KS_KP_F3 		0xf293
#define	KS_KP_F4 		0xf294
#define	KS_KP_Home 		0xf295
#define	KS_KP_Left 		0xf296
#define	KS_KP_Up 		0xf297
#define	KS_KP_Right 		0xf298
#define	KS_KP_Down 		0xf299
#define	KS_KP_Prior 		0xf29a
#define	KS_KP_Next 		0xf29b
#define	KS_KP_End 		0xf29c
#define	KS_KP_Begin 		0xf29d
#define	KS_KP_Insert 		0xf29e
#define	KS_KP_Delete 		0xf29f

#define	KS_KP_Space 		0xf220
#define	KS_KP_Tab 		0xf209
#define	KS_KP_Enter 		0xf20d
#define	KS_KP_Equal 		0xf23d
#define	KS_KP_Numbersign	0xf223
#define	KS_KP_Multiply 		0xf22a
#define	KS_KP_Add 		0xf22b
#define	KS_KP_Separator 	0xf22c
#define	KS_KP_Subtract 		0xf22d
#define	KS_KP_Decimal 		0xf22e
#define	KS_KP_Divide 		0xf22f
#define	KS_KP_0 		0xf230
#define	KS_KP_1 		0xf231
#define	KS_KP_2 		0xf232
#define	KS_KP_3 		0xf233
#define	KS_KP_4 		0xf234
#define	KS_KP_5 		0xf235
#define	KS_KP_6 		0xf236
#define	KS_KP_7 		0xf237
#define	KS_KP_8 		0xf238
#define	KS_KP_9 		0xf239

/*
 * Group 3 (function)
 */

#define KS_f1			0xf300
#define KS_f2			0xf301
#define KS_f3			0xf302
#define KS_f4			0xf303
#define KS_f5			0xf304
#define KS_f6			0xf305
#define KS_f7			0xf306
#define KS_f8			0xf307
#define KS_f9			0xf308
#define KS_f10			0xf309
#define KS_f11			0xf30a
#define KS_f12			0xf30b
#define KS_f13			0xf30c
#define KS_f14			0xf30d
#define KS_f15			0xf30e
#define KS_f16			0xf30f
#define KS_f17			0xf310
#define KS_f18			0xf311
#define KS_f19			0xf312
#define KS_f20			0xf313

#define KS_F1			0xf340
#define KS_F2			0xf341
#define KS_F3			0xf342
#define KS_F4			0xf343
#define KS_F5			0xf344
#define KS_F6			0xf345
#define KS_F7			0xf346
#define KS_F8			0xf347
#define KS_F9			0xf348
#define KS_F10			0xf349
#define KS_F11			0xf34a
#define KS_F12			0xf34b
#define KS_F13			0xf34c
#define KS_F14			0xf34d
#define KS_F15			0xf34e
#define KS_F16			0xf34f
#define KS_F17			0xf350
#define KS_F18			0xf351
#define KS_F19			0xf352
#define KS_F20			0xf353

#define KS_Home			0xf381
#define KS_Prior		0xf382
#define KS_Next			0xf383
#define KS_Up			0xf384
#define KS_Down			0xf385
#define KS_Left			0xf386
#define KS_Right		0xf387
#define KS_End			0xf388
#define KS_Insert		0xf389
#define KS_Help			0xf38a
#define KS_Execute		0xf38b
#define KS_Find			0xf38c
#define KS_Select		0xf38d
#define KS_Again		0xf38e
#define KS_Props		0xf38f
#define KS_Undo			0xf390
#define KS_Front		0xf391
#define KS_Copy			0xf392
#define KS_Open			0xf393
#define KS_Paste		0xf394
#define KS_Cut			0xf395

#define KS_Menu			0xf3c0
#define KS_Pause		0xf3c1
#define KS_Print_Screen		0xf3c2

#define KS_AudioMute		0xf3d1
#define KS_AudioLower		0xf3d2
#define KS_AudioRaise		0xf3d3

/*
 * Group 4 (command)
 */

#define KS_Cmd_Screen0		0xf400
#define KS_Cmd_Screen1		0xf401
#define KS_Cmd_Screen2		0xf402
#define KS_Cmd_Screen3		0xf403
#define KS_Cmd_Screen4		0xf404
#define KS_Cmd_Screen5		0xf405
#define KS_Cmd_Screen6		0xf406
#define KS_Cmd_Screen7		0xf407
#define KS_Cmd_Screen8		0xf408
#define KS_Cmd_Screen9		0xf409
#define KS_Cmd_Screen10		0xf40a
#define KS_Cmd_Screen11		0xf40b
#define KS_Cmd_Debugger		0xf420
#define KS_Cmd_ResetEmul	0xf421
#define KS_Cmd_ResetClose	0xf422
#define KS_Cmd_BacklightOn	0xf423
#define KS_Cmd_BacklightOff	0xf424
#define KS_Cmd_BacklightToggle	0xf425
#define KS_Cmd_BrightnessUp	0xf426
#define KS_Cmd_BrightnessDown	0xf427
#define KS_Cmd_BrightnessRotate	0xf428
#define KS_Cmd_ContrastUp	0xf429
#define KS_Cmd_ContrastDown	0xf42a
#define KS_Cmd_ContrastRotate	0xf42b
#define KS_Cmd_ScrollBack	0xf42c
#define KS_Cmd_ScrollFwd	0xf42d
#define KS_Cmd_KbdReset		0xf42e

/*
 * Group 5 (internal)
 */

#define KS_voidSymbol		0xf500

/*ENDKEYSYMDECL*/

/*
 * keysym groups
 */

#define KS_GROUP_Mod		0xf100
#define KS_GROUP_Keypad		0xf200
#define KS_GROUP_Function	0xf300
#define KS_GROUP_Command	0xf400
#define KS_GROUP_Internal	0xf500
#define KS_GROUP_Dead		0xf801		/* not encoded in keysym */
#define KS_GROUP_Ascii		0xf802		/* not encoded in keysym */
#define KS_GROUP_Keycode	0xf803		/* not encoded in keysym */

#define KS_NUMKEYCODES	0x1000
#define KS_KEYCODE(v)	((v) | 0xe000)

#define KS_GROUP(k)	((k) >= 0x0300 && (k) < 0x0370 ? KS_GROUP_Dead : \
			    (((k) & 0xf000) == 0xe000 ? KS_GROUP_Keycode : \
			      (((k) & 0xf800) == 0xf000 ? ((k) & 0xff00) : \
				KS_GROUP_Ascii)))

#define KS_VALUE(k)	(((k) & 0xf000) == 0xe000 ? ((k) & 0x0fff) : \
			    (((k) & 0xf800) == 0xf000 ? ((k) & 0x00ff) : (k)))

/*
 * Keyboard types: 8bit encoding, 24bit variant
 */

#define KB_ENCODING(e)		((e) & 0x0000ff00)
#define KB_VARIANT(e)		((e) & 0xffff00ff)

#define	KB_NONE			0x0000
#define KB_USER			0x0100
#define KB_US			0x0200
#define KB_DE			0x0300
#define KB_DK			0x0400
#define KB_IT			0x0500
#define KB_FR			0x0600
#define KB_UK			0x0700
#define KB_JP			0x0800
#define KB_SV			0x0900
#define KB_NO			0x0a00
#define KB_ES			0x0b00
#define KB_HU			0x0c00
#define KB_BE			0x0d00
#define KB_RU			0x0e00
#define KB_SG			0x0f00
#define KB_SF			0x1000
#define KB_PT			0x1100
#define KB_UA			0x1200
#define KB_LT			0x1300
#define KB_LA			0x1400
#define KB_BR			0x1500
#define KB_NL			0x1600
#define KB_TR			0x1700
#define KB_PL			0x1800
#define KB_SI			0x1900
#define KB_CF			0x1a00
#define KB_LV			0x1b00

#define KB_NODEAD		0x000001 /* disable dead accents */
#define KB_DECLK		0x000002 /* DEC LKnnn layout */
#define KB_LK401		0x000004 /* DEC LK401 instead LK201 */
#define KB_SWAPCTRLCAPS		0x000008 /* swap Left-Control and Caps-Lock */
#define KB_DVORAK		0x000010 /* Dvorak layout */
#define KB_METAESC		0x000020 /* generate ESC prefix on ALT-key */
#define KB_IOPENER		0x000040 /* f1-f12 -> ESC,f1-f11 */
#define KB_MACHDEP		0x000080 /* machine dependent */
#define	KB_APPLE		0x010000 /* Apple specific layout */
#define KB_COLMAK		0x020000 /* colmak layout */
#define KB_INTL			0x040000 /* International layout */

#define KB_ENCTAB \
	{ KB_USER,	"user" }, \
	{ KB_US,	"us" }, \
	{ KB_DE,	"de" }, \
	{ KB_DK,	"dk" }, \
	{ KB_IT,	"it" }, \
	{ KB_FR,	"fr" }, \
	{ KB_UK,	"uk" }, \
	{ KB_JP,	"jp" }, \
	{ KB_SV,	"sv" }, \
	{ KB_NO,	"no" }, \
	{ KB_ES,	"es" }, \
	{ KB_HU,	"hu" }, \
	{ KB_BE,	"be" }, \
	{ KB_RU,	"ru" }, \
	{ KB_UA,	"ua" }, \
	{ KB_SG,	"sg" }, \
	{ KB_SF,	"sf" }, \
	{ KB_PT,	"pt" }, \
	{ KB_LT,	"lt" }, \
	{ KB_LA,	"la" }, \
	{ KB_BR,	"br" },	\
	{ KB_NL,	"nl" }, \
	{ KB_TR,	"tr" }, \
	{ KB_PL,	"pl" }, \
	{ KB_SI,	"si" }, \
	{ KB_CF,	"cf" }, \
	{ KB_LV,	"lv" }

#define KB_VARTAB \
	{ KB_NODEAD,	"nodead" }, \
	{ KB_DECLK,	"declk" }, \
	{ KB_LK401,	"lk401" }, \
	{ KB_SWAPCTRLCAPS, "swapctrlcaps" }, \
	{ KB_DVORAK,	"dvorak" }, \
	{ KB_METAESC,	"metaesc" }, \
	{ KB_IOPENER,	"iopener" }, \
	{ KB_MACHDEP,	"machdep" }, \
	{ KB_COLMAK,	"colmak" }, \
	{ KB_INTL,	"intl" }, \
	{ KB_APPLE,	"apple" }

#endif /* !_DEV_WSCONS_WSKSYMDEF_H_ */
