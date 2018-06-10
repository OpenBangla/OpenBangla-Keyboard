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

#include <QDebug>
#include <QRegularExpression>
#include <QString>
#include "log.h"
#include "keycode.h"
#include "Settings.h"
#include "BengaliChars.h"
#include "MethodFixedLayoutModern.h"

MethodFixedLayoutModern::MethodFixedLayoutModern() {
  marks = "`~!@#$%^+*-_=+\\|\"/;:,./?><()[]{}";
}

void MethodFixedLayoutModern::setLayout(QJsonObject lay) {
  parser.setLayout(lay);
}

void MethodFixedLayoutModern::internalBackspace() {
  QString BT = BengaliT.mid(0, BengaliT.length()-1);
  BengaliT = BT;
}

void MethodFixedLayoutModern::backspace(QString &string, int step) {
  string = string.mid(0, string.length()-step);
}

void MethodFixedLayoutModern::insertKar(QString kar) {
  /* Automatic Fix of Chandra Position */
  if(gSettings->getAutoChandraPosFixed() && BengaliT.right(1) == b_Chandra) {
    internalBackspace();
    BengaliT = BengaliT + kar + b_Chandra;
  } else if(gSettings->getTraditionalKarFixed() && isPureConsonent(BengaliT.right(1))) {
    /* Traditional Kar Joining */
    /* In UNICODE it is known as "Blocking Bengali Consonant-Vowel Ligature" */
    BengaliT = BengaliT + ZWNJ + kar;
  } else {
    BengaliT += kar;
  }
}

void MethodFixedLayoutModern::insertReph() {
  bool rephMoveable = false;

  bool encounteredConstant = false;
  bool encounteredVowel = false;
  bool encounteredHasanta = false;
  bool encounteredChandra = false;

  if(gSettings->getOldReph()) {
    if(isPureConsonent(BengaliT.right(1))) {
      rephMoveable = true;
    } else if(isVowel(BengaliT.right(1))) {
      if(isPureConsonent(BengaliT.right(2).left(1))) {
        rephMoveable = true;
      } else {
        rephMoveable = false;
      }
    } else if(BengaliT.right(1) == b_Chandra) {
      if(isPureConsonent(BengaliT.right(2).left(1))) {
        rephMoveable = true;
      } else if(isVowel(BengaliT.right(2).left(1)) && isPureConsonent(BengaliT.right(3).left(1))) {
        rephMoveable = true;
      } else {
        rephMoveable = false;
      }
    }
  } else {
    rephMoveable = false;
  }

  if(rephMoveable) {
    QString cache = BengaliT;
    int step = 0;

    for(int i = 1; i <= cache.length(); i++) {
      QString letter = cache.right(i).left(1);

      if(isPureConsonent(letter)) {
        if(encounteredConstant && !encounteredHasanta) break;
        encounteredConstant = true;
        encounteredHasanta = false; // reset
        step++;
        continue;
      } else if(letter == b_Hasanta) {
        encounteredHasanta = true;
        step++;
        continue;
      } else if(isVowel(letter)) {
        if(encounteredVowel) break;

        if((i == 1) || encounteredChandra) {
          encounteredVowel = true;
          step++;
          continue;
        }

        break;
      } else if(letter == b_Chandra) {
        if(i == 1) {
          encounteredChandra = true;
          step++;
          continue;
        }
        break;
      }
    }

    QString temp = cache.right(step);
    backspace(BengaliT, step);
    BengaliT = BengaliT + b_R + b_Hasanta + temp;
    return;
  } else {
    BengaliT = BengaliT + b_R + b_Hasanta;
    return;
  }
}

void MethodFixedLayoutModern::processKeyPress(QString word) {
  /* Automatic Vowel Forming */
  if(gSettings->getAutoVowelFormFixed() && (BengaliT.length() == 0 || isVowel(BengaliT.right(1)) || marks.contains(BengaliT.right(1)))) {
    if(word == b_AAkar) {
      BengaliT += b_AA;
      updateCache();
      return;
    } else if(word == b_Ikar) {
      BengaliT += b_I;
      updateCache();
      return;
    } else if(word == b_IIkar) {
      BengaliT += b_II;
      updateCache();
      return;
    } else if(word == b_Ukar) {
      BengaliT += b_U;
      updateCache();
      return;
    } else if(word == b_UUkar) {
      BengaliT += b_UU;
      updateCache();
      return;
    } else if(word == b_RRIkar) {
      BengaliT += b_RRI;
      updateCache();
      return;
    } else if(word == b_Ekar) {
      BengaliT += b_E;
      updateCache();
      return;
    } else if(word == b_OIkar) {
      BengaliT += b_OI;
      updateCache();
      return;
    } else if(word == b_Okar) {
      BengaliT += b_O;
      updateCache();
      return;
    } else if(word == b_OUkar) {
      BengaliT += b_OU;
      updateCache();
      return;
    }
  }

  if(BengaliT.right(1) == b_Hasanta) {
    /* Vowel making with Hasanta + Kar */
    if(word == b_AAkar) {
      internalBackspace();
      BengaliT += b_AA;
      updateCache();
      return;
    } else if(word == b_Ikar) {
      internalBackspace();
      BengaliT += b_I;
      updateCache();
      return;
    } else if(word == b_IIkar) {
      internalBackspace();
      BengaliT += b_II;
      updateCache();
      return;
    } else if(word == b_Ukar) {
      internalBackspace();
      BengaliT += b_U;
      updateCache();
      return;
    } else if(word == b_UUkar) {
      internalBackspace();
      BengaliT += b_UU;
      updateCache();
      return;
    } else if(word == b_RRIkar) {
      internalBackspace();
      BengaliT += b_RRI;
      updateCache();
      return;
    } else if(word == b_Ekar) {
      internalBackspace();
      BengaliT += b_E;
      updateCache();
      return;
    } else if(word == b_OIkar) {
      internalBackspace();
      BengaliT += b_OI;
      updateCache();
      return;
    } else if(word == b_Okar) {
      internalBackspace();
      BengaliT += b_O;
      updateCache();
      return;
    } else if(word == b_OUkar) {
      internalBackspace();
      BengaliT += b_OU;
      updateCache();
      return;
    } else if(word == b_Hasanta) {
      BengaliT += ZWNJ;
      updateCache();
      return;
    }
    /* Zo fola */
    if(word == b_Z && BengaliT.right(2).left(1) != b_Hasanta) {
      internalBackspace();
      if(BengaliT.right(2).left(1) == b_R) {
        BengaliT = BengaliT + ZWJ + b_Hasanta + b_Z;
      } else {
        BengaliT = BengaliT + b_Hasanta + b_Z;
      }
      updateCache();
      return;
    }
  }

  /* Zo fola */
  if(word == QString(b_Hasanta) + QString(b_Z)) {
    if(BengaliT.right(1) == b_R) {
      BengaliT = BengaliT + ZWJ + word;
    } else {
      BengaliT = BengaliT + word;
    }
    updateCache();
    return;
  }

  /* Reph */
  if(word == QString(b_R) + QString(b_Hasanta)) {
    insertReph();
    updateCache();
    return;
  }

  if(isKar(word)) {
    insertKar(word);
    updateCache();
    return;
  }

  BengaliT += word;
  updateCache();
}

void MethodFixedLayoutModern::updateCache() {
  std::vector<std::string> v;
  v.push_back(BengaliT.toStdString());
  // Now ready the suggestion
  suggest = {v, "", false, 0};
}

Suggestion MethodFixedLayoutModern::getSuggestion(int key, bool shift, bool ctrl, bool alt) {
  // Set modifiers
  bool altgr, shiftaltgr;
  // Don't catch Ctrl without Shift
  if(ctrl && !shift) { handledKey = false; return {}; }

  if(ctrl && alt) { altgr = true; } else { altgr = false; }
  if(shift && altgr) { shiftaltgr = true; } else { shiftaltgr = false; }

  if((key == VC_SHIFT_R || key == VC_SHIFT_L) && BengaliT != "") {
    handledKey = true; return suggest;
  }

  QString pressed = parser.getCharForKey(key, shift, altgr, shiftaltgr);
  if(pressed == "") {
    handledKey = false;
    return {};
  } else {
    processKeyPress(pressed);
    handledKey = true;
    return suggest;
  }
}

Suggestion MethodFixedLayoutModern::getCandidates() {
  return suggest;
}

bool MethodFixedLayoutModern::handledKeyPress() {
  return handledKey;
}

void MethodFixedLayoutModern::candidateCommited(int index) {
  // Clear cache & stored suggestion
  BengaliT = "";
  suggest = {};
}

IMCommand MethodFixedLayoutModern::handleSpecialKey(int key) {
  IMCommand ret;
  if((key == VC_ENTER) || (key == VC_KP_ENTER)) {
    ret.accepted = false;
    if(BengaliT.length() > 0) {
      BengaliT = "";
      ret.commit = true;
      return ret;
    } else {
      return ret;
    }
  } else if(key == VC_SPACE) {
    ret.accepted = false;
    if(BengaliT.length() > 0) {
      BengaliT = "";
      ret.commit = true;
      return ret;
    } else {
      return ret;
    }
  } else if(key == VC_BACKSPACE) {
    if(BengaliT.length() > 0) {
      QString BT = BengaliT.mid(0, BengaliT.length()-1);
      BengaliT = BT;
      updateCache();
      ret.needUpdate = true;
      ret.accepted = true;
      if(BengaliT.length() <= 0) {
        ret.needReset = true;
      }
      return ret;
    } else {
      ret.accepted = false;
      return ret;
    }
  } else if((key == VC_UP) || (key == VC_DOWN) || (key == VC_RIGHT) || (key == VC_LEFT) || (key == VC_TAB)) {
    ret.accepted = false;
    if(BengaliT.length() > 0) {
      BengaliT = "";
      return ret;
    } else {
      return ret;
    }
  }
}
