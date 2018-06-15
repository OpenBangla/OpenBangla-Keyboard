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

#include "keycode.h"
#include "Settings.h"
#include "FixedLayoutParser.h"

void FixedLayoutParser::setLayout(QJsonObject l) {
  layout = l;
}

QString FixedLayoutParser::processAlphabetKey(int keyNum, bool shift, bool altgr, bool shiftaltgr) {
  QString key;
  switch(keyNum) {
    // Begin Alphabet Zone
   case VC_A:
      key = "A";
      break;
   case VC_B:
      key = "B";
      break;
   case VC_C:
      key = "C";
      break;
   case VC_D:
      key = "D";
      break;
   case VC_E:
      key = "E";
      break;
   case VC_F:
      key = "F";
      break;
   case VC_G:
      key = "G";
      break;
   case VC_H:
      key = "H";
      break;
   case VC_I:
      key = "I";
      break;
   case VC_J:
      key = "J";
      break;
   case VC_K:
      key = "K";
      break;
   case VC_L:
      key = "L";
      break;
   case VC_M:
      key = "M";
      break;
   case VC_N:
      key = "N";
      break;
   case VC_O:
      key = "O";
      break;
   case VC_P:
      key = "P";
      break;
   case VC_Q:
      key = "Q";
      break;
   case VC_R:
      key = "R";
      break;
   case VC_S:
      key = "S";
      break;
   case VC_T:
      key = "T";
      break;
   case VC_U:
      key = "U";
      break;
   case VC_V:
      key = "V";
      break;
   case VC_W:
      key = "W";
      break;
   case VC_X:
      key = "X";
      break;
   case VC_Y:
      key = "Y";
      break;
   case VC_Z:
      key = "Z";
      break;

   default:
      return QString("");
  }

  QString mod;
  if(!shift && !altgr && !shiftaltgr) {
    mod = "Normal";
  } else if(shift) {
    mod = "Shift";
  } else if(altgr) {
    mod = "AltGr";
  } else if(shiftaltgr) {
    mod = "ShiftAltGr";
  }

  return QString("Key_%1_%2").arg(key).arg(mod);
}

QString FixedLayoutParser::getCharForKey(int keyNum, bool shift, bool altgr, bool shiftaltgr) {
  QString result = processAlphabetKey(keyNum, shift, altgr, shiftaltgr);
  if(result != "") {
    return layout.value(result).toString();
  }

  QString key;
  switch(keyNum) {
    // Begin Alphanumeric Zone
    case VC_GRAVE:
      key = "BackQuote_Normal";
      break;
    case VC_TILDE:
      key = "BackQuote_Shift";
      break;

    case VC_1:
      key = "1_Normal";
      break;
    case VC_EXCLAIM:
      key = "1_Shift";
      break;
    case VC_2:
      key = "2_Normal";
      break;
    case VC_AT:
      key = "2_Shift";
      break;
    case VC_3:
      key = "3_Normal";
      break;
    case VC_HASH:
      key = "3_Shift";
      break;
    case VC_4:
      key = "4_Normal";
      break;
    case VC_DOLLAR:
      key = "4_Shift";
      break;
    case VC_5:
      key = "5_Normal";
      break;
    case VC_PERCENT:
      key = "5_Shift";
      break;
    case VC_6:
      key = "6_Normal";
      break;
    case VC_CIRCUM:
      key = "6_Shift";
      break;
    case VC_7:
      key = "7_Normal";
      break;
    case VC_AMPERSAND:
      key = "7_Shift";
      break;
    case VC_8:
      key = "8_Normal";
      break;
    case VC_ASTERISK:
      key = "8_Shift";
      break;
    case VC_9:
      key = "9_Normal";
      break;
    case VC_PAREN_LEFT:
      key = "9_Shift";
      break;
   case VC_0:
      key = "0_Normal";
      break;
    case VC_PAREN_RIGHT:
      key = "0_Shift";
      break;
   case VC_MINUS:
      key = "Minus_Normal";
      break;
    case VC_UNDERSCORE:
      key = "Minus_Shift";
   case VC_EQUALS:
      key = "Equals_Normal";
      break;
    case VC_PLUS:
      key = "Equals_Shift";
      break;

   case VC_BRACKET_LEFT:
      key = "OpenBracket_Normal";
      break;
    case VC_BRACE_LEFT:
      key = "OpenBracket_Shift";
      break;
   case VC_BRACKET_RIGHT:
      key = "CloseBracket_Normal";
      break;
    case VC_BRACE_RIGHT:
      key = "CloseBracket_Shift";
      break;
   case VC_BACK_SLASH:
      key = "BlackSlash_Normal";
      break;
    case VC_BAR:
      key = "BlackSlash_Shift";
      break;
   case VC_SEMICOLON:
      key = "Semicolon_Normal";
      break;
   case VC_COLON:
      key = "Semicolon_Shift";
      break;
   case VC_APOSTROPHE:
      key = "Quote_Normal";
      break;
    case VC_QUOTE:
      key = "Quote_Shift";
      break;

   case VC_COMMA:
      key = "Comma_Normal";
      break;
    case VC_LESS:
      key = "Comma_Shift";
      break;
   case VC_PERIOD:
      key = "Period_Normal";
      break;
    case VC_GREATER:
      key = "Period_Shift";
   case VC_SLASH:
      key = "Slash_Normal";
      break;
    case VC_QUESTION:
      key = "Slash_Shift";
      break;
   // End Alphanumeric Zone

   // Begin Numeric Zone
   case VC_KP_DIVIDE:
      key = "NumDivide";
      break;
   case VC_KP_MULTIPLY:
      key = "NumMultiply";
      break;
   case VC_KP_SUBTRACT:
      key = "NumSubtract";
      break;
   case VC_KP_ADD:
      key = "NumAdd";
      break;
   case VC_KP_DECIMAL:
      key = "NumDecimal";
      break;

   case VC_KP_1:
      key = "Num1";
      break;
   case VC_KP_2:
      key = "Num2";
      break;
   case VC_KP_3:
      key = "Num3";
      break;
   case VC_KP_4:
      key= "Num4";
      break;
   case VC_KP_5:
      key = "Num5";
      break;
   case VC_KP_6:
      key = "Num6";
      break;
   case VC_KP_7:
      key = "Num7";
      break;
   case VC_KP_8:
      key = "Num8";
      break;
   case VC_KP_9:
      key = "Num9";
      break;
   case VC_KP_0:
      key = "Num0";
      break;
   // End Numeric Zone

   case VC_UNKNOWN:
   default:
      return QString("");
  }

  QString keyName;
  if(!key.contains("Num")) {
    keyName = QString("Key_%1").arg(key);
  } else {
    if(gSettings->getNumberPadFixed()) {
      keyName = key;
    } else { return QString(""); }
  }

  return layout.value(keyName).toString();
}
