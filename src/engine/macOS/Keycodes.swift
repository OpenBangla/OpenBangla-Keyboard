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

import libEngine

let KeyMap = [
  0x00: VC_A,
  0x01: VC_S,
  0x02: VC_D,
  0x03: VC_F,
  0x04: VC_H,
  0x05: VC_G,
  0x06: VC_Z,
  0x07: VC_X,
  0x08: VC_C,
  0x09: VC_V,
  0x0B: VC_B,
  0x0C: VC_Q,
  0x0D: VC_W,
  0x0E: VC_E,
  0x0F: VC_R,
  0x10: VC_Y,
  0x11: VC_T,
  0x12: VC_1,
  0x13: VC_2,
  0x14: VC_3,
  0x15: VC_4,
  0x16: VC_6,
  0x17: VC_5,
  0x18: VC_EQUALS,
  0x19: VC_9,
  0x1A: VC_7,
  0x1B: VC_MINUS,
  0x1C: VC_8,
  0x1D: VC_0,
  0x1E: VC_OPEN_BRACKET,
  0x1F: VC_O,
  0x20: VC_U,
  0x21: VC_CLOSE_BRACKET,
  0x22: VC_I,
  0x23: VC_P,
  0x25: VC_L,
  0x26: VC_J,
  0x27: VC_QUOTE,
  0x28: VC_K,
  0x29: VC_SEMICOLON,
  0x2A: VC_BACK_SLASH,
  0x2B: VC_COMMA,
  0x2C: VC_SLASH,
  0x2D: VC_N,
  0x2E: VC_M,
  0x2F: VC_PERIOD,
  0x32: VC_BACKQUOTE, // Confusion
  0x41: VC_KP_DECIMAL,
  0x43: VC_KP_MULTIPLY,
  0x45: VC_KP_ADD,
  //kVK_ANSI_KeypadClear          = 0x47,
  0x4B: VC_KP_DIVIDE,
  0x4C: VC_KP_ENTER,
  0x4E: VC_KP_SUBTRACT,
  0x51: VC_KP_EQUALS,
  0x52: VC_KP_0,
  0x53: VC_KP_1,
  0x54: VC_KP_2,
  0x55: VC_KP_3,
  0x56: VC_KP_4,
  0x57: VC_KP_5,
  0x58: VC_KP_6,
  0x59: VC_KP_7,
  0x5B: VC_KP_8,
  0x5C: VC_KP_9
]

func getKeycode(key: Int) -> Int {
  if let value = KeyMap[key] {
    return Int(value)
  } else {
    return Int(VC_UNKNOWN)
  }
}
