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

#pragma once

/* From The Unicode Standard 8.0                *
 * http://www.unicode.org/charts/               *
 * http://www.unicode.org/charts/PDF/U0980.pdf  *
 */

/* The Bengali script is also known as Bangla. In Assam, the   *
 * preferred name of the script is Asamiya or Assamese. The    *
 * Assamese language has also been written historically using  *
 * distinct regional scripts known as Kamrupi.                 *
 */

/* We have changed some Character names according to Bangla language. Actual names are denoted by comments */

/* Various signs */
const char *bSign_Anji = "\u0980";
const char *b_Chandra = "\u0981";
const char *b_Anushar = "\u0982"; // BENGALI SIGN ANUSVARA
const char *b_Bisharga = "\u0983"; // BENGALI SIGN VISARGA

/* Independent vowels */
const char *b_A = "\u0985";
const char *b_AA = "\u0986";
const char *b_I = "\u0987";
const char *b_II = "\u0988";
const char *b_U = "\u0989";
const char *b_UU = "\u098A";
const char *b_RRI = "\u098B"; // BENGALI LETTER VOCALIC R
const char *bVocalic_L = "\u098C";
const char *b_E = "\u098F";
const char *b_OI = "\u0990"; // BENGALI LETTER AI
const char *b_O = "\u0993";
const char *b_OU = "\u0994";

/* Consonants */
const char *b_K = "\u0995";
const char *b_Kh = "\u0996";
const char *b_G = "\u0997";
const char *b_Gh = "\u0998";
const char *b_NGA = "\u0999";
const char *b_C = "\u099A";
const char *b_Ch = "\u099B";
const char *b_J = "\u099C";
const char *b_Jh = "\u099D";
const char *b_NYA = "\u099E";
const char *b_Tt = "\u099F";
const char *b_Tth = "\u09A0";
const char *b_Dd = "\u09A1";
const char *b_Ddh = "\u09A2";
const char *b_Nn = "\u09A3";
const char *b_T = "\u09A4";
const char *b_Th = "\u09A5";
const char *b_D = "\u09A6";
const char *b_Dh = "\u09A7";
const char *b_N = "\u09A8";
const char *b_P = "\u09AA";
const char *b_Ph = "\u09AB";
const char *b_B = "\u09AC";
const char *b_Bh = "\u09AD";
const char *b_M = "\u09AE";
const char *b_Z = "\u09AF";
const char *b_R = "\u09B0";
const char *b_L = "\u09B2";
const char *b_Sh = "\u09B6";
const char *b_Ss = "\u09B7";
const char *b_S = "\u09B8";
const char *b_H = "\u09B9";

/* Various signs */
const char *bSign_Nukta = "\u09BC"; // for extending the alphabet to new letters
const char *bSign_Avagraha = "\u09BD";

/* Dependent vowel signs (kars) */
const char *b_AAkar = "\u09BE";
const char *b_Ikar = "\u09BF";
const char *b_IIkar = "\u09C0";
const char *b_Ukar = "\u09C1";
const char *b_UUkar = "\u09C2";
const char *b_RRIkar = "\u09C3"; // BENGALI VOWEL SIGN VOCALIC R
const char *bVocalic_RR = "\u09C4"; // BENGALI VOWEL SIGN VOCALIC RR
const char *b_OIkar = "\u09C8";

/* Two-part dependent vowel signs */
const char *b_Okar = "\u09CB";
const char *b_OUkar = "\u09CC"; // BENGALI VOWEL SIGN AU

/* Virama or Hasant */
const char *b_Hasanta = "\u09CD";

/* Additional consonant */
const char *b_Khandatta = "\u09CE";

/* Sign */
const char *bLength_Mark = "\u09D7"; // BENGALI AU LENGTH MARK

/* Additional consonants */
const char *b_Rr = "\u09DC";
const char *b_Rh = "\u09DD";
const char *b_Y = "\u09DF";

/* Digits */
const char *b_0 = "\u09E6";
const char *b_1 = "\u09E7";
const char *b_2 = "\u09E8";
const char *b_3 = "\u09E9";
const char *b_4 = "\u09EA";
const char *b_5 = "\u09EB";
const char *b_6 = "\u09EC";
const char *b_7 = "\u09ED";
const char *b_8 = "\u09EE";
const char *b_9 = "\u09EF";

/* Additions for Assamese */
const char *b_RM = "\u09F0"; // BENGALI LETTER RA WITH MIDDLE DIAGONAL
const char *b_RL = "\u09F1"; // BENGALI LETTER RA WITH LOWER DIAGONAL

/* Currency signs */
const char *b_CRTaka_M = "\u09F2"; // BENGALI RUPEE MARK = taka
const char *b_CRTaka = "\u09F3"; // BENGALI RUPEE SIGN = Bangladeshi taka

/* Historic symbols for fractional values */
const char *bCurrencyNumerator_One = "\u09F4";
const char *bCurrencyNumerator_Two = "\u09F5";
const char *bCurrencyNumerator_Three = "\u09F6";
const char *bCurrencyNumerator_Four = "\u09F7";
const char *bCurrencyNumerator_Less = "\u09F8";
const char *bCurrencyNumerator_Sixteen = "\u09F9";

/* Sign */
const char *bSign_Isshar = "\u09FA";

/* Historic currency sign */
const char *bCurrencyGanda = "\u09FB";
const char *
