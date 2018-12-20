/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2018 Muhammad Mominul Huque <mominul2082@gmail.com>
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
#include "MethodPhonetic.h"
#include "Settings.h"

void MethodPhonetic::setLayout(QJsonObject lay) {
  suggest.setLayout(lay);
}

std::vector<std::string> MethodPhonetic::toStdVector(QStringList vec) {
  std::vector<std::string> v;
  v.reserve(vec.size());
  for (auto &str : vec) {
    v.push_back(str.toStdString());
  }
  return v;
}

Suggestion MethodPhonetic::createSuggestion() {
  // Our Suggestion builder
  if (EnglishT == "") {
    // If there is no text available, don't do anything
    return {};
  } else {
    // Reset
    changedCandidateSelection = false;

    // Build the suggestions
    list = suggest.Suggest(EnglishT);

    prevSelected = 0;

    // Check if user wants the candidate window, otherwise don't show it.
    if (gSettings->getShowCWPhonetic()) {
      // Is user selected any candidate before?
      QString selected = suggest.getPrevSelected();
      if (selected != "") {
        // User has selected a candidate before
        int index = list.indexOf(selected);
        if (index != -1) {
          prevSelected = index;
        }
      }

      suggested = {toStdVector(list), EnglishT.toStdString(), true, prevSelected};
    } else {
      // The first candidate will be shown.
      suggested = {toStdVector(list), EnglishT.toStdString(), false, prevSelected};
    }

    return suggested;
  }
}

Suggestion MethodPhonetic::getSuggestion(int key, bool shift, bool ctrl, bool alt) {
  // Don't catch Ctrl or Alt
  if (ctrl || alt) {
    handledKey = false;
    return {};
  }

  switch (key) {
    // Begin Alphanumeric Zone
  case VC_TILDE:EnglishT += "~";
    handledKey = true;
    return createSuggestion();
  case VC_GRAVE:EnglishT += "`";
    handledKey = true;
    return createSuggestion();

  case VC_EXCLAIM:EnglishT += "!";
    handledKey = true;
    return createSuggestion();
  case VC_1:EnglishT += "1";
    handledKey = true;
    return createSuggestion();
  case VC_AT:EnglishT += "@";
    handledKey = true;
    return createSuggestion();
  case VC_2:EnglishT += "2";
    handledKey = true;
    return createSuggestion();
  case VC_HASH:EnglishT += "#";
    handledKey = true;
    return createSuggestion();
  case VC_3:EnglishT += "3";
    handledKey = true;
    return createSuggestion();
  case VC_DOLLAR:EnglishT += "$";
    handledKey = true;
    return createSuggestion();
  case VC_4:EnglishT += "4";
    handledKey = true;
    return createSuggestion();
  case VC_PERCENT:EnglishT += "%";
    handledKey = true;
    return createSuggestion();
  case VC_5:EnglishT += "5";
    handledKey = true;
    return createSuggestion();
  case VC_CIRCUM:EnglishT += "^";
    handledKey = true;
    return createSuggestion();
  case VC_6:EnglishT += "6";
    handledKey = true;
    return createSuggestion();
  case VC_AMPERSAND:EnglishT += "&";
    handledKey = true;
    return createSuggestion();
  case VC_7:EnglishT += "7";
    handledKey = true;
    return createSuggestion();
  case VC_ASTERISK:EnglishT += "*";
    handledKey = true;
    return createSuggestion();
  case VC_8:EnglishT += "8";
    handledKey = true;
    return createSuggestion();
  case VC_PAREN_LEFT:EnglishT += "(";
    handledKey = true;
    return createSuggestion();
  case VC_9:EnglishT += "9";
    handledKey = true;
    return createSuggestion();
  case VC_PAREN_RIGHT:EnglishT += ")";
    handledKey = true;
    return createSuggestion();
  case VC_0:EnglishT += "0";
    handledKey = true;
    return createSuggestion();

  case VC_UNDERSCORE:EnglishT += "_";
    handledKey = true;
    return createSuggestion();
  case VC_MINUS:EnglishT += "-";
    handledKey = true;
    return createSuggestion();
  case VC_PLUS:EnglishT += "+";
    handledKey = true;
    return createSuggestion();
  case VC_EQUALS:EnglishT += "=";
    handledKey = true;
    return createSuggestion();

  case VC_A:
    if (shift) {
      EnglishT += "A";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "a";
      handledKey = true;
      return createSuggestion();
    }
  case VC_B:
    if (shift) {
      EnglishT += "B";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "b";
      handledKey = true;
      return createSuggestion();
    }
  case VC_C:
    if (shift) {
      EnglishT += "C";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "c";
      handledKey = true;
      return createSuggestion();
    }
  case VC_D:
    if (shift) {
      EnglishT += "D";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "d";
      handledKey = true;
      return createSuggestion();
    }
  case VC_E:
    if (shift) {
      EnglishT += "E";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "e";
      handledKey = true;
      return createSuggestion();
    }
  case VC_F:
    if (shift) {
      EnglishT += "F";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "f";
      handledKey = true;
      return createSuggestion();
    }
  case VC_G:
    if (shift) {
      EnglishT += "G";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "g";
      handledKey = true;
      return createSuggestion();
    }
  case VC_H:
    if (shift) {
      EnglishT += "H";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "h";
      handledKey = true;
      return createSuggestion();
    }
  case VC_I:
    if (shift) {
      EnglishT += "I";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "i";
      handledKey = true;
      return createSuggestion();
    }
  case VC_J:
    if (shift) {
      EnglishT += "J";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "j";
      handledKey = true;
      return createSuggestion();
    }
  case VC_K:
    if (shift) {
      EnglishT += "K";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "k";
      handledKey = true;
      return createSuggestion();
    }
  case VC_L:
    if (shift) {
      EnglishT += "L";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "l";
      handledKey = true;
      return createSuggestion();
    }
  case VC_M:
    if (shift) {
      EnglishT += "M";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "m";
      handledKey = true;
      return createSuggestion();
    }
  case VC_N:
    if (shift) {
      EnglishT += "N";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "n";
      handledKey = true;
      return createSuggestion();
    }
  case VC_O:
    if (shift) {
      EnglishT += "O";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "o";
      handledKey = true;
      return createSuggestion();
    }
  case VC_P:
    if (shift) {
      EnglishT += "P";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "p";
      handledKey = true;
      return createSuggestion();
    }
  case VC_Q:
    if (shift) {
      EnglishT += "Q";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "q";
      handledKey = true;
      return createSuggestion();
    }
  case VC_R:
    if (shift) {
      EnglishT += "R";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "r";
      handledKey = true;
      return createSuggestion();
    }
  case VC_S:
    if (shift) {
      EnglishT += "S";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "s";
      handledKey = true;
      return createSuggestion();
    }
  case VC_T:
    if (shift) {
      EnglishT += "T";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "t";
      handledKey = true;
      return createSuggestion();
    }
  case VC_U:
    if (shift) {
      EnglishT += "U";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "u";
      handledKey = true;
      return createSuggestion();
    }
  case VC_V:
    if (shift) {
      EnglishT += "V";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "v";
      handledKey = true;
      return createSuggestion();
    }
  case VC_W:
    if (shift) {
      EnglishT += "W";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "w";
      handledKey = true;
      return createSuggestion();
    }
  case VC_X:
    if (shift) {
      EnglishT += "X";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "x";
      handledKey = true;
      return createSuggestion();
    }
  case VC_Y:
    if (shift) {
      EnglishT += "Y";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "y";
      handledKey = true;
      return createSuggestion();
    }
  case VC_Z:
    if (shift) {
      EnglishT += "Z";
      handledKey = true;
      return createSuggestion();
    } else if (!shift) {
      EnglishT += "z";
      handledKey = true;
      return createSuggestion();
    }

  case VC_BRACE_LEFT:EnglishT += "{";
    handledKey = true;
    return createSuggestion();
  case VC_BRACKET_LEFT:EnglishT += "[";
    handledKey = true;
    return createSuggestion();
  case VC_BRACE_RIGHT:EnglishT += "}";
    handledKey = true;
    return createSuggestion();
  case VC_BRACKET_RIGHT:EnglishT += "]";
    handledKey = true;
    return createSuggestion();
  case VC_BAR:EnglishT += "|";
    handledKey = true;
    return createSuggestion();
  case VC_BACK_SLASH:EnglishT += "\\";
    handledKey = true;
    return createSuggestion();

  case VC_COLON:EnglishT += ":";
    handledKey = true;
    return createSuggestion();
  case VC_SEMICOLON:EnglishT += ";";
    handledKey = true;
    return createSuggestion();
  case VC_QUOTE:EnglishT += "\"";
    handledKey = true;
    return createSuggestion();
  case VC_APOSTROPHE:EnglishT += "\'";
    handledKey = true;
    return createSuggestion();
  case VC_LESS:EnglishT += "<";
    handledKey = true;
    return createSuggestion();
  case VC_COMMA:EnglishT += ",";
    handledKey = true;
    return createSuggestion();
  case VC_GREATER:EnglishT += ">";
    handledKey = true;
    return createSuggestion();
  case VC_PERIOD:EnglishT += ".";
    handledKey = true;
    return createSuggestion();
  case VC_QUESTION:EnglishT += "?";
    handledKey = true;
    return createSuggestion();
  case VC_SLASH:EnglishT += "/";
    handledKey = true;
    return createSuggestion();
    // End Alphanumeric Zone

    // Begin Numeric Zone
  case VC_KP_DIVIDE:
    if (!shift) {
      EnglishT += "/";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }
  case VC_KP_MULTIPLY:
    if (!shift) {
      EnglishT += "*";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }
  case VC_KP_SUBTRACT:
    if (!shift) {
      EnglishT += "-";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }
  case VC_KP_ADD:
    if (!shift) {
      EnglishT += "+";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }

  case VC_KP_DECIMAL:
    if (!shift) {
      EnglishT += ".";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }

  case VC_KP_1:
    if (!shift) {
      EnglishT += "1";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }
  case VC_KP_2:
    if (!shift) {
      EnglishT += "2";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }
  case VC_KP_3:
    if (!shift) {
      EnglishT += "3";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }
  case VC_KP_4:
    if (!shift) {
      EnglishT += "4";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }
  case VC_KP_5:
    if (!shift) {
      EnglishT += "5";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }

  case VC_KP_6:
    if (!shift) {
      EnglishT += "6";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }
  case VC_KP_7:
    if (!shift) {
      EnglishT += "7";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }
  case VC_KP_8:
    if (!shift) {
      EnglishT += "8";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }
  case VC_KP_9:
    if (!shift) {
      EnglishT += "9";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }
  case VC_KP_0:
    if (!shift) {
      EnglishT += "0";
      handledKey = true;
      return createSuggestion();
    } else {
      handledKey = false;
      return {};
    }
    // End Numeric Zone
  case VC_SHIFT:
    if (EnglishT != "") {
      handledKey = true;
      return suggested;
    }
    break; // Have a break
  default:handledKey = false;
    return {};
  }
  // If we have got an break
  handledKey = false;
  return {};
}

Suggestion MethodPhonetic::getCandidates() {
  return suggested;
}

bool MethodPhonetic::handledKeyPress() {
  return handledKey;
}

void MethodPhonetic::candidateCommited(int index) {
  if (changedCandidateSelection) {
    // User selected other candidates
    suggest.saveSelection(index);
    changedCandidateSelection = false;
  }
  // Clear cache & stored suggestions
  EnglishT = "";
  suggested = {};
}

IMCommand MethodPhonetic::handleSpecialKey(int key) {
  IMCommand ret;
  if ((key == VC_ENTER) || (key == VC_KP_ENTER)) {
    if (EnglishT.length() > 0) {
      EnglishT = "";
      ret.commit = true;
      ret.accepted = gSettings->getEnterKeyClosesPrevWin();
      return ret;
    } else {
      ret.accepted = false;
      return ret;
    }
  } else if (key == VC_SPACE) {
    if (EnglishT.length() > 0) {
      ret.commit = true;
      ret.accepted = false;
      EnglishT = "";
      return ret;
    } else {
      ret.accepted = false;
      return ret;
    }
  } else if (key == VC_BACKSPACE) {
    if (EnglishT.length() > 0) {
      QString ET = EnglishT.mid(0, EnglishT.length() - 1);
      EnglishT = ET;
      suggested = createSuggestion();
      ret.needUpdate = true;
      ret.accepted = true;
      if (EnglishT.length() <= 0) {
        ret.needReset = true;
      }
      return ret;
    } else {
      ret.accepted = false;
      return ret;
    }
  } else if ((key == VC_UP) || (key == VC_DOWN)) {
    if ((EnglishT.length() > 0) && !gSettings->getCandidateWinHorizontal()) {
      ret.accepted = true;
      changedCandidateSelection = true;
      return ret;
    } else {
      if (EnglishT.length() > 0)
        EnglishT = "";
      ret.accepted = false;
      return ret;
    }
  } else if ((key == VC_RIGHT) || (key == VC_LEFT)) {
    if ((EnglishT.length() > 0) && gSettings->getCandidateWinHorizontal()) {
      ret.accepted = true;
      changedCandidateSelection = true;
      return ret;
    } else {
      if (EnglishT.length() > 0)
        EnglishT = "";
      ret.accepted = false;
      return ret;
    }
  } else if (key == VC_TAB) {
    if (EnglishT.length() > 0) {
      ret.accepted = true;
      changedCandidateSelection = true;
      return ret;
    } else {
      ret.accepted = false;
      return ret;
    }
  }

  // Control flow should not reach here.
  return ret;
}

void MethodPhonetic::updateEngine() {
  suggest.updateEngine();
}
