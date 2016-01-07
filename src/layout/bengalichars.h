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

#ifndef BENGALICHARS_H
#define BENGALICHARS_H

#include <string>

/* The Unicode Standard 8.0
 * http://www.unicode.org/charts/
 * http://www.unicode.org/charts/PDF/U0980.pdf
 */

/***************************************************************
 * The Bengali script is also known as Bangla. In Assam, the   *
 * preferred name of the script is Asamiya or Assamese. The    *
 * Assamese language has also been written historically using  *
 * distinct regional scripts known as Kamrupi.                 *
 ***************************************************************/

/* We have changed some Character names according to Bangla language or phonetic equivalent.
 * Actual names are denoted by comments */

/* Various signs */
std::string bSign_Anji = "\u0980";
std::string b_Chandra = "\u0981";
std::string b_Anushar = "\u0982"; // BENGALI SIGN ANUSVARA
std::string b_Bisharga = "\u0983"; // BENGALI SIGN VISARGA

/* Independent vowels */
std::string b_A = "\u0985";
std::string b_AA = "\u0986";
std::string b_I = "\u0987";
std::string b_II = "\u0988";
std::string b_U = "\u0989";
std::string b_UU = "\u098A";
std::string b_RRI = "\u098B"; // BENGALI LETTER VOCALIC R
std::string bVocalic_L = "\u098C";
std::string b_E = "\u098F";
std::string b_OI = "\u0990"; // BENGALI LETTER AI
std::string b_O = "\u0993";
std::string b_OU = "\u0994";

/* Consonants */
std::string b_K = "\u0995";
std::string b_Kh = "\u0996";
std::string b_G = "\u0997";
std::string b_Gh = "\u0998";
std::string b_NGA = "\u0999";
std::string b_C = "\u099A";
std::string b_Ch = "\u099B";
std::string b_J = "\u099C";
std::string b_Jh = "\u099D";
std::string b_NYA = "\u099E";
std::string b_Tt = "\u099F";
std::string b_Tth = "\u09A0";
std::string b_Dd = "\u09A1";
std::string b_Ddh = "\u09A2";
std::string b_Nn = "\u09A3";
std::string b_T = "\u09A4";
std::string b_Th = "\u09A5";
std::string b_D = "\u09A6";
std::string b_Dh = "\u09A7";
std::string b_N = "\u09A8";
std::string b_P = "\u09AA";
std::string b_Ph = "\u09AB";
std::string b_B = "\u09AC";
std::string b_Bh = "\u09AD";
std::string b_M = "\u09AE";
std::string b_Z = "\u09AF";
std::string b_R = "\u09B0";
std::string b_L = "\u09B2";
std::string b_Sh = "\u09B6";
std::string b_Ss = "\u09B7";
std::string b_S = "\u09B8";
std::string b_H = "\u09B9";

/* Various signs */
std::string bSign_Nukta = "\u09BC"; // for extending the alphabet to new letters
std::string bSign_Avagraha = "\u09BD";

/* Dependent vowel signs (kars) */
std::string b_AAkar = "\u09BE";
std::string b_Ikar = "\u09BF";
std::string b_IIkar = "\u09C0";
std::string b_Ukar = "\u09C1";
std::string b_UUkar = "\u09C2";
std::string b_RRIkar = "\u09C3"; // BENGALI VOWEL SIGN VOCALIC R
std::string bVocalic_RR = "\u09C4"; // BENGALI VOWEL SIGN VOCALIC RR
std::string b_OIkar = "\u09C8";

/* Two-part dependent vowel signs */
std::string b_Okar = "\u09CB";
std::string b_OUkar = "\u09CC"; // BENGALI VOWEL SIGN AU

/* Virama or Hasant */
std::string b_Hasanta = "\u09CD";

/* Additional consonant */
std::string b_Khandatta = "\u09CE";

/* Sign */
std::string bLength_Mark = "\u09D7"; // BENGALI AU LENGTH MARK

/* Additional consonants */
std::string b_Rr = "\u09DC"; // BENGALI LETTER RRA
std::string b_Rh = "\u09DD"; // BENGALI LETTER RHA
std::string b_Y = "\u09DF"; // BENGALI LETTER YYA

/* Additional vowels for Sanskrit */
std::string bSanskrit_RR = "\u09E0"; // BENGALI LETTER VOCALIC RR
std::string bSanskrit_LL = "\u09E1"; // BENGALI LETTER VOCALIC LL
std::string bSign_L = "\u09E2"; // BENGALI VOWEL SIGN VOCALIC L
std::string bSign_LL = "\u09E3"; // BENGALI VOWEL SIGN VOCALIC LL

/* Reserved */
/****************************************************************
 * For viram punctuation, use the generic Indic 0964 and 0965.  *
 * Note that these punctuation marks are referred to as dahri   *
 * and double dahri in Bangla.                                  *
 ****************************************************************/
std::string b_Dari = "\u0964";
std::string b_DDari = "\u0965";

/* Digits */
std::string b_0 = "\u09E6";
std::string b_1 = "\u09E7";
std::string b_2 = "\u09E8";
std::string b_3 = "\u09E9";
std::string b_4 = "\u09EA";
std::string b_5 = "\u09EB";
std::string b_6 = "\u09EC";
std::string b_7 = "\u09ED";
std::string b_8 = "\u09EE";
std::string b_9 = "\u09EF";

/* Additions for Assamese */
std::string b_RM = "\u09F0"; // BENGALI LETTER RA WITH MIDDLE DIAGONAL
std::string b_RL = "\u09F1"; // BENGALI LETTER RA WITH LOWER DIAGONAL

/* Currency signs */
std::string b_CRTaka_M = "\u09F2"; // BENGALI RUPEE MARK = taka
std::string b_CRTaka = "\u09F3"; // BENGALI RUPEE SIGN = Bangladeshi taka

/* Historic symbols for fractional values */
std::string bCurrencyNumerator_One = "\u09F4";
std::string bCurrencyNumerator_Two = "\u09F5";
std::string bCurrencyNumerator_Three = "\u09F6";
std::string bCurrencyNumerator_Four = "\u09F7";
std::string bCurrencyNumerator_Less = "\u09F8";
std::string bCurrencyNumerator_Sixteen = "\u09F9";

/* Sign */
std::string bSign_Isshar = "\u09FA";

/* Historic currency sign */
std::string bCurrencyGanda = "\u09FB";

/* Unicode Addition */
std::string ZWJ = "\u200D";
std::string ZWNJ = "\u200C";

#endif /* end of include guard: BENGALICHARS_H */
