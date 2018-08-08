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

QString FixedLayoutParser::getCharForKey(int keyNum, bool shift, bool altgr, bool shiftaltgr) {
  QString key;
  bool emulateShift = false;
  switch (keyNum) {
    // Begin Alphabet Zone
  case VC_A:key = "A";
    break;
  case VC_B:key = "B";
    break;
  case VC_C:key = "C";
    break;
  case VC_D:key = "D";
    break;
  case VC_E:key = "E";
    break;
  case VC_F:key = "F";
    break;
  case VC_G:key = "G";
    break;
  case VC_H:key = "H";
    break;
  case VC_I:key = "I";
    break;
  case VC_J:key = "J";
    break;
  case VC_K:key = "K";
    break;
  case VC_L:key = "L";
    break;
  case VC_M:key = "M";
    break;
  case VC_N:key = "N";
    break;
  case VC_O:key = "O";
    break;
  case VC_P:key = "P";
    break;
  case VC_Q:key = "Q";
    break;
  case VC_R:key = "R";
    break;
  case VC_S:key = "S";
    break;
  case VC_T:key = "T";
    break;
  case VC_U:key = "U";
    break;
  case VC_V:key = "V";
    break;
  case VC_W:key = "W";
    break;
  case VC_X:key = "X";
    break;
  case VC_Y:key = "Y";
    break;
  case VC_Z:key = "Z";
    break;
    // End Alphabet Zone
  case VC_GRAVE:key = "BackQuote";
    break;
  case VC_TILDE:key = "BackQuote";
    emulateShift = true;
    break;

  case VC_1:key = "1";
    break;
  case VC_EXCLAIM:key = "1";
    emulateShift = true;
    break;
  case VC_2:key = "2";
    break;
  case VC_AT:key = "2";
    emulateShift = true;
    break;
  case VC_3:key = "3";
    break;
  case VC_HASH:key = "3";
    emulateShift = true;
    break;
  case VC_4:key = "4";
    break;
  case VC_DOLLAR:key = "4";
    emulateShift = true;
    break;
  case VC_5:key = "5";
    break;
  case VC_PERCENT:key = "5";
    emulateShift = true;
    break;
  case VC_6:key = "6";
    break;
  case VC_CIRCUM:key = "6";
    emulateShift = true;
    break;
  case VC_7:key = "7";
    break;
  case VC_AMPERSAND:key = "7";
    emulateShift = true;
    break;
  case VC_8:key = "8";
    break;
  case VC_ASTERISK:key = "8";
    emulateShift = true;
    break;
  case VC_9:key = "9";
    break;
  case VC_PAREN_LEFT:key = "9";
    emulateShift = true;
    break;
  case VC_0:key = "0";
    break;
  case VC_PAREN_RIGHT:key = "0";
    emulateShift = true;
    break;
  case VC_MINUS:key = "Minus";
    break;
  case VC_UNDERSCORE:key = "Minus";
    emulateShift = true;
    break;
  case VC_EQUALS:key = "Equals";
    break;
  case VC_PLUS:key = "Equals";
    emulateShift = true;
    break;

  case VC_BRACKET_LEFT:key = "OpenBracket";
    break;
  case VC_BRACE_LEFT:key = "OpenBracket";
    emulateShift = true;
    break;
  case VC_BRACKET_RIGHT:key = "CloseBracket";
    break;
  case VC_BRACE_RIGHT:key = "CloseBracket";
    emulateShift = true;
    break;
  case VC_BACK_SLASH:key = "BlackSlash";
    break;
  case VC_BAR:key = "BlackSlash";
    emulateShift = true;
    break;
  case VC_SEMICOLON:key = "Semicolon";
    break;
  case VC_COLON:key = "Semicolon";
    emulateShift = true;
    break;
  case VC_APOSTROPHE:key = "Quote";
    break;
  case VC_QUOTE:key = "Quote";
    emulateShift = true;
    break;

  case VC_COMMA:key = "Comma";
    break;
  case VC_LESS:key = "Comma";
    emulateShift = true;
    break;
  case VC_PERIOD:key = "Period";
    break;
  case VC_GREATER:key = "Period";
    emulateShift = true;
    break;
  case VC_SLASH:key = "Slash";
    break;
  case VC_QUESTION:key = "Slash";
    emulateShift = true;
    break;

    // Begin Numeric Zone
  case VC_KP_DIVIDE:key = "NumDivide";
    break;
  case VC_KP_MULTIPLY:key = "NumMultiply";
    break;
  case VC_KP_SUBTRACT:key = "NumSubtract";
    break;
  case VC_KP_ADD:key = "NumAdd";
    break;
  case VC_KP_DECIMAL:key = "NumDecimal";
    break;

  case VC_KP_1:key = "Num1";
    break;
  case VC_KP_2:key = "Num2";
    break;
  case VC_KP_3:key = "Num3";
    break;
  case VC_KP_4:key = "Num4";
    break;
  case VC_KP_5:key = "Num5";
    break;
  case VC_KP_6:key = "Num6";
    break;
  case VC_KP_7:key = "Num7";
    break;
  case VC_KP_8:key = "Num8";
    break;
  case VC_KP_9:key = "Num9";
    break;
  case VC_KP_0:key = "Num0";
    break;
    // End Numeric Zone

  case VC_UNKNOWN:
  default:return QString("");
  }

  QString keyName;
  if (!key.contains("Num")) {
    // Check if the key is an alphabet key.
    if (keyNum >= VC_A && keyNum <= VC_Z) {
      // We accept Shift, AltGr and ShiftAltGr when the key
      // is an alphabet key.
      if (!shift && !altgr && !shiftaltgr) {
        keyName = QString("Key_%1_Normal").arg(key);
      } else if (shiftaltgr) {
        keyName = QString("Key_%1_ShiftAltGr").arg(key);
      } else if (altgr) {
        keyName = QString("Key_%1_AltGr").arg(key);
      } else if (shift) {
        keyName = QString("Key_%1_Shift").arg(key);
      }
    } else {
      if (!emulateShift && !altgr) {
        keyName = QString("Key_%1_Normal").arg(key);
      } else if (emulateShift && altgr) {
        keyName = QString("Key_%1_ShiftAltGr").arg(key);
      } else if (altgr) {
        keyName = QString("Key_%1_AltGr").arg(key);
      } else if (emulateShift) {
        keyName = QString("Key_%1_Shift").arg(key);
      }
    }
  } else {
    if (gSettings->getNumberPadFixed()) {
      keyName = key;
    } else {
      return QString("");
    }
  }

  return layout.value(keyName).toString();
}
