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
 * IM sends Keycodes according to this file.
 * When IM gets Keycode or Key event, IM converts them to these Keycodes */

/* From libUIOHook */

// Begin Alphanumeric Zone
// BACKQUTE
#define VC_GRAVE                                0x0029  // '`'
//ESC
#define VC_TILDE                                0x0001  // '~'

#define VC_1                                    0x0002
#define VC_2                                    0x0003
#define VC_3                                    0x0004
#define VC_4                                    0x0005
#define VC_5                                    0x0006
#define VC_6                                    0x0007
#define VC_7                                    0x0008
#define VC_8                                    0x0009
#define VC_9                                    0x000A
#define VC_0                                    0x000B

// F1-F12
#define VC_EXCLAIM                                0x003B
#define VC_AT                                    0x003C
#define VC_HASH                                    0x003D
#define VC_DOLLAR                                0x003E
#define VC_PERCENT                                0x003F
#define VC_CIRCUM                                0x0040
#define VC_AMPERSAND                            0x0041
#define VC_ASTERISK                                0x0042
#define VC_PAREN_LEFT                            0x0043
#define VC_PAREN_RIGHT                            0x0044
#define VC_UNDERSCORE                            0x0057
#define VC_PLUS                                    0x0058

#define VC_MINUS                                0x000C    // '-'
#define VC_EQUALS                                0x000D    // '='
#define VC_BACKSPACE                            0x000E

#define VC_TAB                                    0x000F

#define VC_A                                    0xA0B4
#define VC_B                                    0xA0B5
#define VC_C                                    0xA0B6
#define VC_D                                    0xA0B7
#define VC_E                                    0xA0B8
#define VC_F                                    0xA0B9
#define VC_G                                    0xA0BA
#define VC_H                                    0xA0BB
#define VC_I                                    0xA0BC
#define VC_J                                    0xA0BD
#define VC_K                                    0xA0BE
#define VC_L                                    0xA0BF
#define VC_M                                    0xA0C0
#define VC_N                                    0xA0C1
#define VC_O                                    0xA0C2
#define VC_P                                    0xA0C3
#define VC_Q                                    0xA0C4
#define VC_R                                    0xA0C5
#define VC_S                                    0xA0C6
#define VC_T                                    0xA0C7
#define VC_U                                    0xA0C8
#define VC_V                                    0xA0C9
#define VC_W                                    0xA0CA
#define VC_X                                    0xA0CB
#define VC_Y                                    0xA0CC
#define VC_Z                                    0xA0CD

#define VC_BRACKET_LEFT                            0x001A    // '['
#define VC_BRACKET_RIGHT                        0x001B    // ']'
#define VC_BACK_SLASH                            0x002B    // '\'

//F13-F15
#define VC_BRACE_LEFT                            0x005B  // '{'
#define VC_BRACE_RIGHT                            0x005C  // '}'
#define VC_BAR                                    0x005D  // '|'

#define VC_SEMICOLON                            0x0027    // ';'
#define VC_APOSTROPHE                            0x0028  // '''
#define VC_ENTER                                0x001C

#define VC_COMMA                                0x0033    // ','
#define VC_PERIOD                                0x0034    // '.'
#define VC_SLASH                                0x0035    // '/'

// F16-F20
#define VC_COLON                                0x0063 // ':'
#define VC_QUOTE                                0x0064 // '"'
#define VC_LESS                                    0x0065 // '<'
#define VC_GREATER                                0x0066 // '>'
#define VC_QUESTION                            0x0067 // '?'

#define VC_SPACE                                0x0039
// End Alphanumeric Zone

#define VC_UNKNOWN                                0x0046

// Begin Edit Key Zone
#define VC_INSERT                                0x0E52
#define VC_DELETE                                0x0E53
#define VC_HOME                                    0x0E47
#define VC_END                                    0x0E4F
#define VC_PAGE_UP                                0x0E49
#define VC_PAGE_DOWN                            0x0E51
// End Edit Key Zone


// Begin Cursor Key Zone
#define VC_UP                                    0xE048
#define VC_LEFT                                    0xE04B
#define VC_RIGHT                                0xE04D
#define VC_DOWN                                    0xE050
// End Cursor Key Zone


// Begin Numeric Zone
#define VC_KP_DIVIDE                            0x0E35
#define VC_KP_MULTIPLY                            0x0037
#define VC_KP_SUBTRACT                            0x004A
#define VC_KP_EQUALS                            0x0E0D
#define VC_KP_ADD                                0x004E
#define VC_KP_ENTER                                0x0E1C
#define VC_KP_DECIMAL                            0x0053

#define VC_KP_1                                    0x004F
#define VC_KP_2                                    0x0050
#define VC_KP_3                                    0x0051
#define VC_KP_4                                    0x004B
#define VC_KP_5                                    0x004C
#define VC_KP_6                                    0x004D
#define VC_KP_7                                    0x0047
#define VC_KP_8                                    0x0048
#define VC_KP_9                                    0x0049
#define VC_KP_0                                    0x0052
// End Numeric Zone

#define VC_SHIFT                                0x002A
//#define VC_SHIFT_R							0x0036
#define VC_CONTROL                                0x001D
//#define VC_CONTROL_R							0x0E1D
#define VC_ALT                                    0x0038
//#define VC_ALT_R	                            0x0E38

#endif /* end of include guard: IM_KEYCODE_H */
