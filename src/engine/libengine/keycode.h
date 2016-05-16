/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015 Muhammad Mominul Huque <mominul2082@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IM_KEYCODE_H
#define IM_KEYCODE_H

/* Cross-platform Keycodes for OpenBangla Keyboard
 * IM sends Keycodes accroding to this file.
 * When IM gets Keycode or Key event, IM converts them to these Keycodes */

/* From libUIOHook */

// Begin Alphanumeric Zone
#define VC_BACKQUOTE							0x0029  // '`` , '~'

#define VC_1									0x0002
#define VC_2									0x0003
#define VC_3									0x0004
#define VC_4									0x0005
#define VC_5									0x0006
#define VC_6									0x0007
#define VC_7									0x0008
#define VC_8									0x0009
#define VC_9									0x000A
#define VC_0									0x000B

#define VC_MINUS								0x000C	// '-' , '_'
#define VC_EQUALS								0x000D	// '=' , '+'
#define VC_BACKSPACE							0x000E

#define VC_TAB									0x000F

#define VC_A									0x001E
#define VC_B									0x0030
#define VC_C									0x002E
#define VC_D									0x0020
#define VC_E									0x0012
#define VC_F									0x0021
#define VC_G									0x0022
#define VC_H									0x0023
#define VC_I									0x0017
#define VC_J									0x0024
#define VC_K									0x0025
#define VC_L									0x0026
#define VC_M									0x0032
#define VC_N									0x0031
#define VC_O									0x0018
#define VC_P									0x0019
#define VC_Q									0x0010
#define VC_R									0x0013
#define VC_S									0x001F
#define VC_T									0x0014
#define VC_U									0x0016
#define VC_V									0x002F
#define VC_W									0x0011
#define VC_X									0x002D
#define VC_Y									0x0015
#define VC_Z									0x002C

#define VC_OPEN_BRACKET							0x001A	// '[' , '{'
#define VC_CLOSE_BRACKET						0x001B	// ']' , '}'
#define VC_BACK_SLASH							0x002B	// '\' , '|'

#define VC_SEMICOLON							0x0027	// ';'
#define VC_QUOTE								0x0028 // ''' , '"'
#define VC_ENTER								0x001C

#define VC_COMMA								0x0033	// ',' , '<'
#define VC_PERIOD								0x0034	// '.' , '>'
#define VC_SLASH								0x0035	// '/' , '?'

#define VC_SPACE								0x0039
// End Alphanumeric Zone

#define VC_UNKNOWN							0x0046

// Begin Edit Key Zone
#define VC_INSERT								0x0E52
#define VC_DELETE								0x0E53
#define VC_HOME									0x0E47
#define VC_END									0x0E4F
#define VC_PAGE_UP								0x0E49
#define VC_PAGE_DOWN							0x0E51
// End Edit Key Zone


// Begin Cursor Key Zone
#define VC_UP									0xE048
#define VC_LEFT									0xE04B
#define VC_RIGHT								0xE04D
#define VC_DOWN									0xE050
// End Cursor Key Zone


// Begin Numeric Zone
#define VC_KP_DIVIDE							0x0E35
#define VC_KP_MULTIPLY							0x0037
#define VC_KP_SUBTRACT							0x004A
#define VC_KP_EQUALS							0x0E0D
#define VC_KP_ADD								0x004E
#define VC_KP_ENTER								0x0E1C
#define VC_KP_DECIMAL							0x0053

#define VC_KP_1									0x004F
#define VC_KP_2									0x0050
#define VC_KP_3									0x0051
#define VC_KP_4									0x004B
#define VC_KP_5									0x004C
#define VC_KP_6									0x004D
#define VC_KP_7									0x0047
#define VC_KP_8									0x0048
#define VC_KP_9									0x0049
#define VC_KP_0									0x0052
// End Numeric Zone

#define VC_SHIFT_L								0x002A
#define VC_SHIFT_R								0x0036

#endif /* end of include guard: IM_KEYCODE_H */
