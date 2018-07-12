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

#include <QChar>

class QString;

class QRegularExpression;

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
QChar bSign_Anji = 0x980;
QChar b_Chandra = 0x981;
QChar b_Anushar = 0x982; // BENGALI SIGN ANUSVARA
QChar b_Bisharga = 0x983; // BENGALI SIGN VISARGA

/* Independent vowels */
QChar b_A = 0x985;
QChar b_AA = 0x986;
QChar b_I = 0x987;
QChar b_II = 0x988;
QChar b_U = 0x989;
QChar b_UU = 0x98A;
QChar b_RRI = 0x98B; // BENGALI LETTER VOCALIC R
QChar bVocalic_L = 0x98C;
QChar b_E = 0x98F;
QChar b_OI = 0x990; // BENGALI LETTER AI
QChar b_O = 0x993;
QChar b_OU = 0x994;

/* Consonants */
QChar b_K = 0x995;
QChar b_Kh = 0x996;
QChar b_G = 0x997;
QChar b_Gh = 0x998;
QChar b_NGA = 0x999;
QChar b_C = 0x99A;
QChar b_Ch = 0x99B;
QChar b_J = 0x99C;
QChar b_Jh = 0x99D;
QChar b_NYA = 0x99E;
QChar b_Tt = 0x99F;
QChar b_Tth = 0x9A0;
QChar b_Dd = 0x9A1;
QChar b_Ddh = 0x9A2;
QChar b_Nn = 0x9A3;
QChar b_T = 0x9A4;
QChar b_Th = 0x9A5;
QChar b_D = 0x9A6;
QChar b_Dh = 0x9A7;
QChar b_N = 0x9A8;
QChar b_P = 0x9AA;
QChar b_Ph = 0x9AB;
QChar b_B = 0x9AC;
QChar b_Bh = 0x9AD;
QChar b_M = 0x9AE;
QChar b_Z = 0x9AF;
QChar b_R = 0x9B0;
QChar b_L = 0x9B2;
QChar b_Sh = 0x9B6;
QChar b_Ss = 0x9B7;
QChar b_S = 0x9B8;
QChar b_H = 0x9B9;

/* Various signs */
QChar bSign_Nukta = 0x9BC; // for extending the alphabet to new letters
QChar bSign_Avagraha = 0x9BD;

/* Dependent vowel signs (kars) */
QChar b_AAkar = 0x9BE;
QChar b_Ikar = 0x9BF;
QChar b_IIkar = 0x9C0;
QChar b_Ukar = 0x9C1;
QChar b_UUkar = 0x9C2;
QChar b_RRIkar = 0x9C3; // BENGALI VOWEL SIGN VOCALIC R
QChar bVocalic_RR = 0x9C4; // BENGALI VOWEL SIGN VOCALIC RR
QChar b_Ekar = 0x9C7;
QChar b_OIkar = 0x9C8;

/* Two-part dependent vowel signs */
QChar b_Okar = 0x9CB;
QChar b_OUkar = 0x9CC; // BENGALI VOWEL SIGN AU

/* Virama or Hasant */
QChar b_Hasanta = 0x9CD;

/* Additional consonant */
QChar b_Khandatta = 0x9CE;

/* Sign */
QChar bLength_Mark = 0x9D7; // BENGALI AU LENGTH MARK

/* Additional consonants */
QChar b_Rr = 0x9DC; // BENGALI LETTER RRA
QChar b_Rh = 0x9DD; // BENGALI LETTER RHA
QChar b_Y = 0x9DF; // BENGALI LETTER YYA

/* Additional vowels for Sanskrit */
QChar bSanskrit_RR = 0x9E0; // BENGALI LETTER VOCALIC RR
QChar bSanskrit_LL = 0x9E1; // BENGALI LETTER VOCALIC LL
QChar bSign_L = 0x9E2; // BENGALI VOWEL SIGN VOCALIC L
QChar bSign_LL = 0x9E3; // BENGALI VOWEL SIGN VOCALIC LL

/* Reserved */
/****************************************************************
 * For viram punctuation, use the generic Indic 0964 and 0965.  *
 * Note that these punctuation marks are referred to as dahri   *
 * and double dahri in Bangla.                                  *
 ****************************************************************/
QChar b_Dari = 0x964;
QChar b_DDari = 0x965;

/* Digits */
QChar b_0 = 0x9E6;
QChar b_1 = 0x9E7;
QChar b_2 = 0x9E8;
QChar b_3 = 0x9E9;
QChar b_4 = 0x9EA;
QChar b_5 = 0x9EB;
QChar b_6 = 0x9EC;
QChar b_7 = 0x9ED;
QChar b_8 = 0x9EE;
QChar b_9 = 0x9EF;

/* Additions for Assamese */
QChar b_RM = 0x9F0; // BENGALI LETTER RA WITH MIDDLE DIAGONAL
QChar b_RL = 0x9F1; // BENGALI LETTER RA WITH LOWER DIAGONAL

/* Currency signs */
QChar b_CRTaka_M = 0x9F2; // BENGALI RUPEE MARK = taka
QChar b_CRTaka = 0x9F3; // BENGALI RUPEE SIGN = Bangladeshi taka

/* Historic symbols for fractional values */
QChar bCurrencyNumerator_One = 0x9F4;
QChar bCurrencyNumerator_Two = 0x9F5;
QChar bCurrencyNumerator_Three = 0x9F6;
QChar bCurrencyNumerator_Four = 0x9F7;
QChar bCurrencyNumerator_Less = 0x9F8;
QChar bCurrencyNumerator_Sixteen = 0x9F9;

/* Sign */
QChar bSign_Isshar = 0x9FA;

/* Historic currency sign */
QChar bCurrencyGanda = 0x9FB;

/* Unicode Addition */
QChar ZWJ = 0x200D;
QChar ZWNJ = 0x200C;

bool isKar(QString c) {
    QRegularExpression regex(
            "^[\\x{09BE}\\x{09BF}\\x{09C0}\\x{09C1}\\x{09C2}\\x{09C3}\\x{09C7}\\x{09C8}\\x{09CB}\\x{09CC}\\x{09C4}]$");
    return c.contains(regex);
}

bool isVowel(QString c) {
    QRegularExpression regex(
            "^[\\x{0985}\\x{0986}\\x{0987}\\x{0988}\\x{0989}\\x{098A}\\x{098B}\\x{098F}\\x{0990}\\x{0993}\\x{0994}\\x{098C}\\x{09E1}\\x{09BE}\\x{09BF}\\x{09C0}\\x{09C1}\\x{09C2}\\x{09C3}\\x{09C7}\\x{09C8}\\x{09CB}\\x{09CC}]$");
    return c.contains(regex);
}

bool isPureConsonent(QString c) {
    QRegularExpression regex(
            "^[\\x{0995}\\x{0996}\\x{0997}\\x{0998}\\x{0999}\\x{099A}\\x{099B}\\x{099C}\\x{099D}\\x{099E}\\x{099F}\\x{09A0}\\x{09A1}\\x{09A2}\\x{09A3}\\x{09A4}\\x{09A5}\\x{09A6}\\x{09A7}\\x{09A8}\\x{09AA}\\x{09AB}\\x{09AC}\\x{09AD}\\x{09AE}\\x{09AF}\\x{09B0}\\x{09B2}\\x{09B6}\\x{09B7}\\x{09B8}\\x{09B9}\\x{09CE}\\x{09DC}\\x{09DD}\\x{09DF}]$");
    return c.contains(regex);
}

#endif /* end of include guard: BENGALICHARS_H */
