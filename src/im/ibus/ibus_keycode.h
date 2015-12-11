/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015 Muhammad Mominul Huque <nahidbinbaten1995@gmail.com>
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

#ifndef IBUS_KEYCODE_H
#define IBUS_KEYCODE_H

#include <map>
#include <ibus.h>
#include "im/keycode.h"

/* Here we map iBus keycodes with ours.
 * We use iBus's US layout(https://github.com/ibus/ibus/blob/master/data/keymaps/us)
 */

std::map<guint,int> key;

void intKeycode() {
  // Begin Alphanumeric Zone
  key[IBUS_KEY_0] = VC_0;
  key[IBUS_KEY_1] = VC_1;
  key[IBUS_KEY_2] = VC_2;
  key[IBUS_KEY_3] = VC_3;
  key[IBUS_KEY_4] = VC_4;
  key[IBUS_KEY_5] = VC_5;
  key[IBUS_KEY_6] = VC_6;
  key[IBUS_KEY_7] = VC_7;
  key[IBUS_KEY_8] = VC_8;
  key[IBUS_KEY_9] = VC_9;

  // If these occured, emulate shift key
  key[IBUS_KEY_parenright] = VC_0;
  key[IBUS_KEY_exclam] = VC_1;
  key[IBUS_KEY_at] = VC_2;
  key[IBUS_KEY_numbersign] = VC_3;
  key[IBUS_KEY_dollar] = VC_4;
  key[IBUS_KEY_percent] = VC_5;
  key[IBUS_KEY_asciicircum] = VC_6;
  key[IBUS_KEY_ampersand] = VC_7;
  key[IBUS_KEY_asterisk] = VC_8;
  key[IBUS_KEY_parenleft] = VC_9;

  /* You may notice that we have mapped
   * uppercase and lowercase keys with same
   * keys, becase we would emulate shift key
   * when needed.
   */
  // If occured, emulate shift key
  key[IBUS_KEY_A] = VC_A;
  key[IBUS_KEY_B] = VC_B;
  key[IBUS_KEY_C] = VC_C;
  key[IBUS_KEY_D] = VC_D;
  key[IBUS_KEY_E] = VC_E;
  key[IBUS_KEY_F] = VC_F;
  key[IBUS_KEY_G] = VC_G;
  key[IBUS_KEY_H] = VC_H;
  key[IBUS_KEY_I] = VC_I;
  key[IBUS_KEY_J] = VC_J;
  key[IBUS_KEY_K] = VC_K;
  key[IBUS_KEY_L] = VC_L;
  key[IBUS_KEY_M] = VC_M;
  key[IBUS_KEY_N] = VC_N;
  key[IBUS_KEY_O] = VC_O;
  key[IBUS_KEY_P] = VC_P;
  key[IBUS_KEY_Q] = VC_Q;
  key[IBUS_KEY_R] = VC_R;
  key[IBUS_KEY_S] = VC_S;
  key[IBUS_KEY_T] = VC_T;
  key[IBUS_KEY_U] = VC_U;
  key[IBUS_KEY_V] = VC_V;
  key[IBUS_KEY_W] = VC_W;
  key[IBUS_KEY_X] = VC_X;
  key[IBUS_KEY_Y] = VC_Y;
  key[IBUS_KEY_Z] = VC_Z;

  // If occured, send without shift key
  key[IBUS_KEY_a] = VC_A;
  key[IBUS_KEY_b] = VC_B;
  key[IBUS_KEY_c] = VC_C;
  key[IBUS_KEY_d] = VC_D;
  key[IBUS_KEY_e] = VC_E;
  key[IBUS_KEY_f] = VC_F;
  key[IBUS_KEY_g] = VC_G;
  key[IBUS_KEY_h] = VC_H;
  key[IBUS_KEY_i] = VC_I;
  key[IBUS_KEY_J] = VC_J;
  key[IBUS_KEY_k] = VC_K;
  key[IBUS_KEY_l] = VC_L;
  key[IBUS_KEY_m] = VC_M;
  key[IBUS_KEY_n] = VC_N;
  key[IBUS_KEY_o] = VC_O;
  key[IBUS_KEY_p] = VC_P;
  key[IBUS_KEY_q] = VC_Q;
  key[IBUS_KEY_r] = VC_R;
  key[IBUS_KEY_s] = VC_S;
  key[IBUS_KEY_t] = VC_T;
  key[IBUS_KEY_u] = VC_U;
  key[IBUS_KEY_v] = VC_V;
  key[IBUS_KEY_w] = VC_W;
  key[IBUS_KEY_x] = VC_X;
  key[IBUS_KEY_y] = VC_Y;
  key[IBUS_KEY_z] = VC_Z;

  key[IBUS_KEY_bracketleft] = VC_OPEN_BRACKET;
  key[IBUS_KEY_braceleft] = VC_OPEN_BRACKET;

  key[IBUS_KEY_bracketright] = VC_CLOSE_BRACKET;
  key[IBUS_KEY_braceright] = VC_CLOSE_BRACKET;

  key[IBUS_KEY_backslash] = VC_BACK_SLASH;
  key[IBUS_KEY_bar] = VC_BACK_SLASH;

  key[IBUS_KEY_slash] = VC_SLASH;
  key[IBUS_KEY_question] = VC_SLASH;

  key[IBUS_KEY_semicolon] = VC_SEMICOLON;
  key[IBUS_KEY_colon] = VC_SEMICOLON;

  key[IBUS_KEY_comma] = VC_COMMA;
  key[IBUS_KEY_less] = VC_COMMA;

  key[IBUS_KEY_period] = VC_PERIOD;
  key[IBUS_KEY_greater] = VC_PERIOD;

  key[IBUS_KEY_apostrophe] = VC_QUOTE;
  key[IBUS_KEY_quotedbl] = VC_QUOTE;

  key[IBUS_KEY_Return] = VC_ENTER;
  key[IBUS_KEY_space] = VC_SPACE;

  // Begin Cursor Key Zone
  key[IBUS_KEY_Left] = VC_LEFT;
  key[IBUS_KEY_Up] = VC_UP;
  key[IBUS_KEY_Right] = VC_RIGHT;
  key[IBUS_KEY_Down] = VC_DOWN;
  // End Cursor Key Zone

  // Begin Numeric Zone
  key[IBUS_KEY_KP_Divide] = VC_KP_DIVIDE;
  key[IBUS_KEY_KP_Multiply] = VC_KP_MULTIPLY;
  key[IBUS_KEY_KP_Subtract] = VC_KP_SUBTRACT;
  key[IBUS_KEY_KP_Equal] = VC_KP_EQUALS;
  key[IBUS_KEY_KP_Add] = VC_KP_ADD;
  key[IBUS_KEY_KP_Enter] = VC_KP_ENTER;
  key[IBUS_KEY_KP_Separator] = VC_KP_SEPARATOR;

  key[IBUS_KEY_KP_1] = VC_KP_1;
  key[IBUS_KEY_KP_2] = VC_KP_2;
  key[IBUS_KEY_KP_3] = VC_KP_3;
  key[IBUS_KEY_KP_4] = VC_KP_4;
  key[IBUS_KEY_KP_5] = VC_KP_5;
  key[IBUS_KEY_KP_6] = VC_KP_6;
  key[IBUS_KEY_KP_7] = VC_KP_7;
  key[IBUS_KEY_KP_8] = VC_KP_8;
  key[IBUS_KEY_KP_9] = VC_KP_9;
  key[IBUS_KEY_KP_0] = VC_KP_0;
  // End Numeric Zone
}

int ibus_keycode(guint k) {
  intKeycode();
  try
  {
    return key.at(k);
  }
  catch(std::out_of_range)
  {
    // The key is not mapped - means that we don't needed the UNKNOWN key!
    LOG_DEBUG("[IM:iBus]: Got unknown keycode. Mapping to VC_UNKNOWN");
    return VC_UNKNOWN;
  }
}

#endif /* end of include guard: IBUS_KEYCODE_H */
