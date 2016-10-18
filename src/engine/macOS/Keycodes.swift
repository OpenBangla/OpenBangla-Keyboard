/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2016 Muhammad Mominul Huque <mominul2082@gmail.com>
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

import Carbon
import libEngine

let KeyMap = [
  kVK_ANSI_A: VC_A,
  kVK_ANSI_S: VC_S,
  kVK_ANSI_D: VC_D,
  kVK_ANSI_F: VC_F,
  kVK_ANSI_H: VC_H,
  kVK_ANSI_G: VC_G,
  kVK_ANSI_Z: VC_Z,
  kVK_ANSI_X: VC_X,
  kVK_ANSI_C: VC_C,
  kVK_ANSI_V: VC_V,
  kVK_ANSI_B: VC_B,
  kVK_ANSI_Q: VC_Q,
  kVK_ANSI_W: VC_W,
  kVK_ANSI_E: VC_E,
  kVK_ANSI_R: VC_R,
  kVK_ANSI_Y: VC_Y,
  kVK_ANSI_T: VC_T,
  kVK_ANSI_1: VC_1,
  kVK_ANSI_2: VC_2,
  kVK_ANSI_3: VC_3,
  kVK_ANSI_4: VC_4,
  kVK_ANSI_6: VC_6,
  kVK_ANSI_5: VC_5,
  kVK_ANSI_Equal: VC_EQUALS,
  kVK_ANSI_9: VC_9,
  kVK_ANSI_7: VC_7,
  kVK_ANSI_Minus: VC_MINUS,
  kVK_ANSI_8: VC_8,
  kVK_ANSI_0: VC_0,
  kVK_ANSI_RightBracket: VC_OPEN_BRACKET,
  kVK_ANSI_O: VC_O,
  kVK_ANSI_U: VC_U,
  kVK_ANSI_LeftBracket: VC_CLOSE_BRACKET,
  kVK_ANSI_I: VC_I,
  kVK_ANSI_P: VC_P,
  kVK_ANSI_L: VC_L,
  kVK_ANSI_J: VC_J,
  kVK_ANSI_Quote: VC_QUOTE,
  kVK_ANSI_K: VC_K,
  kVK_ANSI_Semicolon: VC_SEMICOLON,
  kVK_ANSI_Backslash: VC_BACK_SLASH,
  kVK_ANSI_Comma: VC_COMMA,
  kVK_ANSI_Slash: VC_SLASH,
  kVK_ANSI_N: VC_N,
  kVK_ANSI_M: VC_M,
  kVK_ANSI_Period: VC_PERIOD,
  kVK_ANSI_Grave: VC_BACKQUOTE, // Confusion
  kVK_ANSI_KeypadDecimal: VC_KP_DECIMAL,
  kVK_ANSI_KeypadMultiply: VC_KP_MULTIPLY,
  kVK_ANSI_KeypadPlus: VC_KP_ADD,
  //kVK_ANSI_KeypadClear          = 0x47,
  kVK_ANSI_KeypadDivide: VC_KP_DIVIDE,
  kVK_ANSI_KeypadEnter: VC_KP_ENTER,
  kVK_ANSI_KeypadMinus: VC_KP_SUBTRACT,
  kVK_ANSI_KeypadEquals: VC_KP_EQUALS,
  kVK_ANSI_Keypad0: VC_KP_0,
  kVK_ANSI_Keypad1: VC_KP_1,
  kVK_ANSI_Keypad2: VC_KP_2,
  kVK_ANSI_Keypad3: VC_KP_3,
  kVK_ANSI_Keypad4: VC_KP_4,
  kVK_ANSI_Keypad5: VC_KP_5,
  kVK_ANSI_Keypad6: VC_KP_6,
  kVK_ANSI_Keypad7: VC_KP_7,
  kVK_ANSI_Keypad8: VC_KP_8,
  kVK_ANSI_Keypad9: VC_KP_9
]

func getKeycode(key: Int) -> Int {
  if let value = KeyMap[key] {
    return Int(value)
  } else {
    return Int(VC_UNKNOWN)
  }
}
