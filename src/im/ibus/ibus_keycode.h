/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015  Muhammad Mominul Huque <nahidbinbaten1995@gmail.com>
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

#pragma once

#include <map>
#include <ibus.h>
#include "im/keycode.h"

/* Here we map iBus keycodes with ours */

std::map<guint,int> key;

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

key[IBUS_KEY_Left] = VC_LEFT;
key[IBUS_KEY_Up] = VC_UP;
key[IBUS_KEY_Right] = VC_RIGHT;
key[IBUS_KEY_Down] = VC_DOWN;

int ibus_keycode(guint k) {
  return key[k];
}
