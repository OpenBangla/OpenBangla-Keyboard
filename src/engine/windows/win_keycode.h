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

#ifndef WIN_KEYCODE_H
#define WIN_KEYCODE_H

#include <map>
#include <windows.h>
#include "log.h"

/* Windows Virtual Keycode Mapping
 * https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx
 */

std::map<int, int> key;

void intKeycode() {
  // Begin Alphanumeric Zone
  key[0x30] = VC_0;
  key[0x31] = VC_1;
  key[0x32] = VC_2;
  key[0x33] = VC_3;
  key[0x34] = VC_4;
  key[0x35] = VC_5;
  key[0x36] = VC_6;
  key[0x37] = VC_7;
  key[0x38] = VC_8;
  key[0x39] = VC_9;

  key[VK_OEM_MINUS] = VC_MINUS;

  key[VK_ADD] = VC_EQUALS;

  key[VK_BACK] = VC_BACKSPACE;

  key[VK_TAB] = VC_TAB;

  key[0x41] = VC_A;
  key[0x42] = VC_B;
  key[0x43] = VC_C;
  key[0x44] = VC_D;
  key[0x45] = VC_E;
  key[0x46] = VC_F;
  key[0x47] = VC_G;
  key[0x48] = VC_H;
  key[0x49] = VC_I;
  key[0x4A] = VC_J;
  key[0x4B] = VC_K;
  key[0x4C] = VC_L;
  key[0x4D] = VC_M;
  key[0x4E] = VC_N;
  key[0x4F] = VC_O;
  key[0x50] = VC_P;
  key[0x51] = VC_Q;
  key[0x52] = VC_R;
  key[0x53] = VC_S;
  key[0x54] = VC_T;
  key[0x55] = VC_U;
  key[0x56] = VC_V;
  key[0x57] = VC_W;
  key[0x58] = VC_X;
  key[0x59] = VC_Y;
  key[0x5A] = VC_Z;

  key[VK_OEM_4] = VC_OPEN_BRACKET;

  key[VK_OEM_6] = VC_CLOSE_BRACKET;

  key[VK_OEM_5] = VC_BACK_SLASH;

  key[VK_OEM_2] = VC_SLASH;

  key[VK_OEM_1] = VC_SEMICOLON;

  key[VK_OEM_COMMA] = VC_COMMA;

  key[VK_OEM_PERIOD] = VC_PERIOD;

  key[VK_OEM_7] = VC_QUOTE;

  key[VK_RETURN] = VC_ENTER;
  key[VK_SPACE] = VC_SPACE;

  // Begin Cursor Key Zone
  key[VK_LEFT] = VC_LEFT;
  key[VK_UP] = VC_UP;
  key[VK_RIGHT] = VC_RIGHT;
  key[VK_DOWN] = VC_DOWN;
  // End Cursor Key Zone

  // Begin Numeric Zone
  key[VK_DIVIDE] = VC_KP_DIVIDE;
  key[VK_MULTIPLY] = VC_KP_MULTIPLY;
  key[VK_SUBTRACT] = VC_KP_SUBTRACT;
  //key[IBUS_KEY_KP_Equal] = VC_KP_EQUALS;
  key[VK_ADD] = VC_KP_ADD;
  //key[IBUS_KEY_KP_Enter] = VC_KP_ENTER;
  key[VK_SEPARATOR] = VC_KP_SEPARATOR;

  key[VK_NUMPAD1] = VC_KP_1;
  key[VK_NUMPAD2] = VC_KP_2;
  key[VK_NUMPAD3] = VC_KP_3;
  key[VK_NUMPAD4] = VC_KP_4;
  key[VK_NUMPAD5] = VC_KP_5;
  key[VK_NUMPAD6] = VC_KP_6;
  key[VK_NUMPAD7] = VC_KP_7;
  key[VK_NUMPAD8] = VC_KP_8;
  key[VK_NUMPAD9] = VC_KP_9;
  key[VK_NUMPAD0] = VC_KP_0;
  // End Numeric Zone
}

int win_keycode(guint k) {
  intKeycode();
  try {
    return key.at(k);
  }
  catch (std::out_of_range) {
    // The key is not mapped - means that we don't needed the UNKNOWN key!
    LOG_DEBUG("[IM:Windows]: Got unknown keycode. Mapping to VC_UNKNOWN");
    return VC_UNKNOWN;
  }
}

#endif /* end of include guard: WIN_KEYCODE_H */
