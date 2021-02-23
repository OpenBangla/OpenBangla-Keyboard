/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2020 Muhammad Mominul Huque <mominul2082@gmail.com>
 *  Copyright (C) 2021 CSSlayer <wengxt@gmail.com>
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

#ifndef KEYCODE_H
#define KEYCODE_H

#include "riti.h"
#include <fcitx-utils/keysym.h>
#include <map>
#include <stdexcept>

/* Here we map Fcitx keycodes with riti's. */
constexpr uint16_t VC_UNKNOWN = 0x0046;

uint16_t keySymToRitiKey(FcitxKeySym k) {
  static const std::map<FcitxKeySym, uint16_t> key = {
      // Begin Alphanumeric Zone
      {FcitxKey_grave, VC_GRAVE},
      {FcitxKey_dead_grave, VC_GRAVE},
      {FcitxKey_asciitilde, VC_TILDE},

      {FcitxKey_0, VC_0},
      {FcitxKey_1, VC_1},
      {FcitxKey_2, VC_2},
      {FcitxKey_3, VC_3},
      {FcitxKey_4, VC_4},
      {FcitxKey_5, VC_5},
      {FcitxKey_6, VC_6},
      {FcitxKey_7, VC_7},
      {FcitxKey_8, VC_8},
      {FcitxKey_9, VC_9},

      {FcitxKey_parenright, VC_PAREN_RIGHT},
      {FcitxKey_exclam, VC_EXCLAIM},
      {FcitxKey_at, VC_AT},
      {FcitxKey_numbersign, VC_HASH},
      {FcitxKey_dollar, VC_DOLLAR},
      {FcitxKey_percent, VC_PERCENT},
      {FcitxKey_asciicircum, VC_CIRCUM},
      {FcitxKey_ampersand, VC_AMPERSAND},
      {FcitxKey_asterisk, VC_ASTERISK},
      {FcitxKey_parenleft, VC_PAREN_LEFT},

      {FcitxKey_minus, VC_MINUS},
      {FcitxKey_underscore, VC_UNDERSCORE},

      {FcitxKey_equal, VC_EQUALS},
      {FcitxKey_plus, VC_PLUS},

      {FcitxKey_A, VC_A_SHIFT},
      {FcitxKey_B, VC_B_SHIFT},
      {FcitxKey_C, VC_C_SHIFT},
      {FcitxKey_D, VC_D_SHIFT},
      {FcitxKey_E, VC_E_SHIFT},
      {FcitxKey_F, VC_F_SHIFT},
      {FcitxKey_G, VC_G_SHIFT},
      {FcitxKey_H, VC_H_SHIFT},
      {FcitxKey_I, VC_I_SHIFT},
      {FcitxKey_J, VC_J_SHIFT},
      {FcitxKey_K, VC_K_SHIFT},
      {FcitxKey_L, VC_L_SHIFT},
      {FcitxKey_M, VC_M_SHIFT},
      {FcitxKey_N, VC_N_SHIFT},
      {FcitxKey_O, VC_O_SHIFT},
      {FcitxKey_P, VC_P_SHIFT},
      {FcitxKey_Q, VC_Q_SHIFT},
      {FcitxKey_R, VC_R_SHIFT},
      {FcitxKey_S, VC_S_SHIFT},
      {FcitxKey_T, VC_T_SHIFT},
      {FcitxKey_U, VC_U_SHIFT},
      {FcitxKey_V, VC_V_SHIFT},
      {FcitxKey_W, VC_W_SHIFT},
      {FcitxKey_X, VC_X_SHIFT},
      {FcitxKey_Y, VC_Y_SHIFT},
      {FcitxKey_Z, VC_Z_SHIFT},

      {FcitxKey_a, VC_A},
      {FcitxKey_b, VC_B},
      {FcitxKey_c, VC_C},
      {FcitxKey_d, VC_D},
      {FcitxKey_e, VC_E},
      {FcitxKey_f, VC_F},
      {FcitxKey_g, VC_G},
      {FcitxKey_h, VC_H},
      {FcitxKey_i, VC_I},
      {FcitxKey_j, VC_J},
      {FcitxKey_k, VC_K},
      {FcitxKey_l, VC_L},
      {FcitxKey_m, VC_M},
      {FcitxKey_n, VC_N},
      {FcitxKey_o, VC_O},
      {FcitxKey_p, VC_P},
      {FcitxKey_q, VC_Q},
      {FcitxKey_r, VC_R},
      {FcitxKey_s, VC_S},
      {FcitxKey_t, VC_T},
      {FcitxKey_u, VC_U},
      {FcitxKey_v, VC_V},
      {FcitxKey_w, VC_W},
      {FcitxKey_x, VC_X},
      {FcitxKey_y, VC_Y},
      {FcitxKey_z, VC_Z},

      {FcitxKey_bracketleft, VC_BRACKET_LEFT},
      {FcitxKey_braceleft, VC_BRACE_LEFT},

      {FcitxKey_bracketright, VC_BRACKET_RIGHT},
      {FcitxKey_braceright, VC_BRACE_RIGHT},

      {FcitxKey_backslash, VC_BACK_SLASH},
      {FcitxKey_bar, VC_BAR},

      {FcitxKey_slash, VC_SLASH},
      {FcitxKey_question, VC_QUESTION},

      {FcitxKey_semicolon, VC_SEMICOLON},
      {FcitxKey_colon, VC_COLON},

      {FcitxKey_comma, VC_COMMA},
      {FcitxKey_less, VC_LESS},

      {FcitxKey_period, VC_PERIOD},
      {FcitxKey_greater, VC_GREATER},

      {FcitxKey_apostrophe, VC_APOSTROPHE},
      {FcitxKey_quotedbl, VC_QUOTE},

      // Begin Numeric Zone
      {FcitxKey_KP_Divide, VC_KP_DIVIDE},
      {FcitxKey_KP_Multiply, VC_KP_MULTIPLY},
      {FcitxKey_KP_Subtract, VC_KP_SUBTRACT},
      {FcitxKey_KP_Equal, VC_KP_EQUALS},
      {FcitxKey_KP_Add, VC_KP_ADD},
      {FcitxKey_KP_Enter, VC_KP_ENTER},
      {FcitxKey_KP_Decimal, VC_KP_DECIMAL},

      {FcitxKey_KP_1, VC_KP_1},
      {FcitxKey_KP_2, VC_KP_2},
      {FcitxKey_KP_3, VC_KP_3},
      {FcitxKey_KP_4, VC_KP_4},
      {FcitxKey_KP_5, VC_KP_5},
      {FcitxKey_KP_6, VC_KP_6},
      {FcitxKey_KP_7, VC_KP_7},
      {FcitxKey_KP_8, VC_KP_8},
      {FcitxKey_KP_9, VC_KP_9},
      {FcitxKey_KP_0, VC_KP_0},
      // End Numeric Zone
  };
  if (auto iter = key.find(k); iter != key.end()) {
    return iter->second;
  }
  return VC_UNKNOWN;
}

#endif /* end of include guard: KEYCODE_H */
