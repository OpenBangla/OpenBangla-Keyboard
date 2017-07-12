/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2016 Muhammad Mominul Huque <mominul2082@gmail.com>
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

//#include "im.h"
#include "log.h"
#include "keycode.h"
#include "MethodPhonetic.h"
#include "Settings.h"

void MethodPhonetic::setLayout(QJsonObject lay) {
  suggest.setLayout(lay);
}

std::vector<std::string> MethodPhonetic::toStdVector(QVector<QString> vec) {
  std::vector<std::string> v;
  for(auto& str : vec) {
    v.push_back(str.toStdString());
  }
  return v;
}

Suggestion MethodPhonetic::createSuggestion() {
  // Our Suggestion builder
  if(EnglishT == "") {
    // If there is no text available, don't do anything
    return {};
  } else {/*
    if(changedCandidateSelection) {
      // User selected other candidate, save current selection
      suggest.saveSelection(QString::fromStdString(im_get_selection(im_get_selection_id())));
      changedCandidateSelection = false;
    }*/

    // Build the suggestions
    list = suggest.Suggest(EnglishT).toVector();

    // Is user selected any candidate before?
    prevSelected = 0;
    QString selected = suggest.getPrevSelected();
    if(selected != "") {
      // User has selected a candidate before
      int index = list.indexOf(selected);
      if(index != -1) {
        prevSelected = index;
      }
    }

    suggested = {toStdVector(list), EnglishT.toStdString(), true, prevSelected};
    return suggested;
  }
}

Suggestion MethodPhonetic::getSuggestion(int key, bool shift, bool ctrl, bool alt) {
  // Don't catch Ctrl or Alt
  if(ctrl || alt) { handledKey = false; return {}; }

  switch(key) {
    // Begin Alphanumeric Zone
    case VC_BACKQUOTE:
    if(shift) {
      EnglishT += "~";
      handledKey = true;
      return createSuggestion();
    } else if(!shift) {
      EnglishT += "`";
      handledKey = true;
      return createSuggestion();
    }

    case VC_1:
      if(shift) {
        EnglishT += "!";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "1";
        handledKey = true;
        return createSuggestion();
      }
    case VC_2:
      if(shift) {
        EnglishT += "@";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "2";
        handledKey = true;
        return createSuggestion();
      }
    case VC_3:
      if(shift) {
        EnglishT += "#";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "3";
        handledKey = true;
        return createSuggestion();
      }
    case VC_4:
      if(shift) {
        EnglishT += "$";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "4";
        handledKey = true;
        return createSuggestion();
      }
    case VC_5:
      if(shift) {
        EnglishT += "%";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "5";
        handledKey = true;
        return createSuggestion();
      }
    case VC_6:
      if(shift) {
        EnglishT += "^";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "6";
        handledKey = true;
        return createSuggestion();
      }
    case VC_7:
      if(shift) {
        EnglishT += "&";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "7";
        handledKey = true;
        return createSuggestion();
      }
    case VC_8:
      if(shift) {
        EnglishT += "*";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "8";
        handledKey = true;
        return createSuggestion();
      }
    case VC_9:
      if(shift) {
        EnglishT += "(";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "9";
        handledKey = true;
        return createSuggestion();
      }
   case VC_0:
      if(shift) {
        EnglishT += ")";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "0";
        handledKey = true;
        return createSuggestion();
      }

   case VC_MINUS:
      if(shift) {
        EnglishT += "_";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "-";
        handledKey = true;
        return createSuggestion();
      }
   case VC_EQUALS:
      if(shift) {
        EnglishT += "+";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "=";
        handledKey = true;
        return createSuggestion();
      }

   case VC_A:
      if(shift) {
        EnglishT += "A";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "a";
        handledKey = true;
        return createSuggestion();
      }
   case VC_B:
      if(shift) {
        EnglishT += "B";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "b";
        handledKey = true;
        return createSuggestion();
      }
   case VC_C:
      if(shift) {
        EnglishT += "C";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "c";
        handledKey = true;
        return createSuggestion();
      }
   case VC_D:
      if(shift) {
        EnglishT += "D";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "d";
        handledKey = true;
        return createSuggestion();
      }
   case VC_E:
      if(shift) {
        EnglishT += "E";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "e";
        handledKey = true;
        return createSuggestion();
      }
   case VC_F:
      if(shift) {
        EnglishT += "F";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "f";
        handledKey = true;
        return createSuggestion();
      }
   case VC_G:
      if(shift) {
        EnglishT += "G";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "g";
        handledKey = true;
        return createSuggestion();
      }
   case VC_H:
      if(shift) {
        EnglishT += "H";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "h";
        handledKey = true;
        return createSuggestion();
      }
   case VC_I:
      if(shift) {
        EnglishT += "I";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "i";
        handledKey = true;
        return createSuggestion();
      }
   case VC_J:
      if(shift) {
        EnglishT += "J";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "j";
        handledKey = true;
        return createSuggestion();
      }
   case VC_K:
      if(shift) {
        EnglishT += "K";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "k";
        handledKey = true;
        return createSuggestion();
      }
   case VC_L:
      if(shift) {
        EnglishT += "L";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "l";
        handledKey = true;
        return createSuggestion();
      }
   case VC_M:
      if(shift) {
        EnglishT += "M";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "m";
        handledKey = true;
        return createSuggestion();
      }
   case VC_N:
      if(shift) {
        EnglishT += "N";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "n";
        handledKey = true;
        return createSuggestion();
      }
   case VC_O:
      if(shift) {
        EnglishT += "O";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "o";
        handledKey = true;
        return createSuggestion();
      }
   case VC_P:
      if(shift) {
        EnglishT += "P";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "p";
        handledKey = true;
        return createSuggestion();
      }
   case VC_Q:
      if(shift) {
        EnglishT += "Q";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "q";
        handledKey = true;
        return createSuggestion();
      }
   case VC_R:
      if(shift) {
        EnglishT += "R";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "r";
        handledKey = true;
        return createSuggestion();
      }
   case VC_S:
      if(shift) {
        EnglishT += "S";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "s";
        handledKey = true;
        return createSuggestion();
      }
   case VC_T:
      if(shift) {
        EnglishT += "T";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "t";
        handledKey = true;
        return createSuggestion();
      }
   case VC_U:
      if(shift) {
        EnglishT += "U";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "u";
        handledKey = true;
        return createSuggestion();
      }
   case VC_V:
      if(shift) {
        EnglishT += "V";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "v";
        handledKey = true;
        return createSuggestion();
      }
   case VC_W:
      if(shift) {
        EnglishT += "W";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "w";
        handledKey = true;
        return createSuggestion();
      }
   case VC_X:
      if(shift) {
        EnglishT += "X";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "x";
        handledKey = true;
        return createSuggestion();
      }
   case VC_Y:
      if(shift) {
        EnglishT += "Y";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "y";
        handledKey = true;
        return createSuggestion();
      }
   case VC_Z:
      if(shift) {
        EnglishT += "Z";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "z";
        handledKey = true;
        return createSuggestion();
      }

   case VC_OPEN_BRACKET:
      if(shift) {
        EnglishT += "{";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "[";
        handledKey = true;
        return createSuggestion();
      }
   case VC_CLOSE_BRACKET:
      if(shift) {
        EnglishT += "}";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "]";
        handledKey = true;
        return createSuggestion();
      }
   case VC_BACK_SLASH:
      if(shift) {
        EnglishT += "|";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "\\";
        handledKey = true;
        return createSuggestion();
      }

   case VC_SEMICOLON:
      if(shift) {
        EnglishT += ":";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += ";";
        handledKey = true;
        return createSuggestion();
      }
   case VC_QUOTE:
      if(shift) {
        EnglishT += "\"";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "\'";
        handledKey = true;
        return createSuggestion();
      }
   case VC_COMMA:
      if(shift) {
        EnglishT += "<";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += ",";
        handledKey = true;
        return createSuggestion();
      }
   case VC_PERIOD:
      if(shift) {
        EnglishT += ">";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += ".";
        handledKey = true;
        return createSuggestion();
      }
   case VC_SLASH:
      if(shift) {
        EnglishT += "?";
        handledKey = true;
        return createSuggestion();
      } else if(!shift) {
        EnglishT += "/";
        handledKey = true;
        return createSuggestion();
      }
   // End Alphanumeric Zone

   // Begin Numeric Zone
   case VC_KP_DIVIDE:
      if(!shift) {
        EnglishT += "/";
        handledKey = true;
        return createSuggestion();
      } else { handledKey = false; return {}; }
   case VC_KP_MULTIPLY:
      if(!shift) {
        EnglishT += "*";
        handledKey = true;
        return createSuggestion();
      } else { handledKey = false; return {}; }
   case VC_KP_SUBTRACT:
      if(!shift){
        EnglishT += "-";
        handledKey = true;
        return createSuggestion();
      } else { handledKey = false; return {}; }
   case VC_KP_ADD:
      if(!shift) {
        EnglishT += "+";
        handledKey = true;
        return createSuggestion();
      } else { handledKey = false; return {}; }

   case VC_KP_DECIMAL:
      if(!shift) {
        EnglishT += ".";
        handledKey = true;
        return createSuggestion();
      } else { handledKey = false; return {}; }

   case VC_KP_1:
      if(!shift) {
        EnglishT += "1";
        handledKey = true;
        return createSuggestion();
      } else {
        handledKey = false; return {};
      }
   case VC_KP_2:
      if(!shift) {
        EnglishT += "2";
        handledKey = true;
        return createSuggestion();
      } else {
        handledKey = false; return {};
      }
   case VC_KP_3:
      if(!shift) {
        EnglishT += "3";
        handledKey = true;
        return createSuggestion();
      } else {
        handledKey = false; return {};
      }
   case VC_KP_4:
      if(!shift) {
        EnglishT += "4";
        handledKey = true;
        return createSuggestion();
      } else {
        handledKey = false; return {};
      }
   case VC_KP_5:
      if(!shift) {
        EnglishT += "5";
        handledKey = true;
        return createSuggestion();
      } else {
        handledKey = false;
        return {};
      }

   case VC_KP_6:
      if(!shift) {
        EnglishT += "6";
        handledKey = true; return createSuggestion();
      } else {
        handledKey = false; return {};
      }
   case VC_KP_7:
      if(!shift) {
        EnglishT += "7";
        handledKey = true;
        return createSuggestion();
      } else {
        handledKey = false; return {};
      }
   case VC_KP_8:
      if(!shift) {
        EnglishT += "8";
        handledKey = true;
        return createSuggestion();
      } else {
        handledKey = false; return {};
      }
   case VC_KP_9:
      if(!shift) {
        EnglishT += "9";
        handledKey = true; return createSuggestion();
      } else {
        handledKey = false; return {};
      }
   case VC_KP_0:
      if(!shift) {
        EnglishT += "0";
        handledKey = true; return createSuggestion();
      } else {
        handledKey = false; return {};
      }
   // End Numeric Zone
   case VC_SHIFT_R:
   case VC_SHIFT_L:
      if(EnglishT != "") {
        handledKey = true; return suggested;
      }
      break; // Have a break
   default:
      handledKey = false; return {};
  }
  // If we have got an break
  handledKey = false; return {};
 }

Suggestion MethodPhonetic::getCandidates() {
  return suggested;
}

bool MethodPhonetic::handledKeyPress() {
  return handledKey;
}

void MethodPhonetic::candidateCommited(std::string commited) {
  if(changedCandidateSelection) {
    // User selected other candidates
    suggest.saveSelection(QString::fromStdString(commited));
    changedCandidateSelection = false;
  }
  // Clear cache & stored suggestions
  EnglishT = "";
  suggested = {};
}

IMCommand MethodPhonetic::handleSpecialKey(int key) {
  IMCommand ret;
  if((key == VC_ENTER) || (key == VC_KP_ENTER)) {
    if(EnglishT.length() > 0) {
      EnglishT = "";
      ret.commit = true;
      if(gSettings->getEnterKeyClosesPrevWin()) { ret.accepted = true; } else { ret.accepted = false; }
      return ret;
    } else {
      ret.accepted = false;
      return ret;
    }
  } else if(key == VC_SPACE) {
    if(EnglishT.length() > 0) {
      ret.commit = true;
      ret.accepted = false;
      EnglishT = "";
      return ret;
    } else {
      ret.accepted = false;
      return ret;
    }
  } else if(key == VC_BACKSPACE) {
    if(EnglishT.length() > 0) {
      QString ET = EnglishT.mid(0, EnglishT.length()-1);
      EnglishT = ET;
      suggested = createSuggestion();
      ret.needUpdate = true;
      ret.accepted = true;
      if(EnglishT.length() <= 0) {
        ret.needReset = true;
      }
      return ret;
    } else {
      ret.accepted = false;
      return ret;
    }
  } else if(((key == VC_UP) || (key == VC_DOWN)) && !gSettings->getCandidateWinHorizontal()) {
    if(EnglishT.length() > 0) {
      ret.accepted = true;
      changedCandidateSelection = true;
      return ret;
    } else {
      if(EnglishT.length() > 0) EnglishT = "";
      ret.accepted = false;
      return ret;
    }
  } else if(((key == VC_RIGHT) || (key == VC_LEFT)) && gSettings->getCandidateWinHorizontal()) {
    if(EnglishT.length() > 0) {
      ret.accepted = true;
      changedCandidateSelection = true;
      return ret;
    } else {
      if(EnglishT.length() > 0) EnglishT = "";
      ret.accepted = false;
      return ret;
    }
  } else if(key == VC_TAB) {
    if(EnglishT.length() > 0) {
      ret.accepted = true;
      changedCandidateSelection = true;
      return ret;
    } else {
      ret.accepted = false;
      return ret;
    }
  }
}
