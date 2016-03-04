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
#include "im.h"
#include "BengaliChars.h"
#include "MethodFixedLayoutModern.h"

void MethodFixedLayoutModern::setLayout(QJsonObject lay) {
  parser.setLayout(lay);
}

void MethodFixedLayoutModern::internalBackspace() {
  QString BT = BengaliT.mid(0, BengaliT.length()-1);
  BengaliT = BT;
}

void MethodFixedLayoutModern::makeWord(QString word) {
  /* Vowel making with Hasanta + Kar */
  if(BengaliT.right(1) == b_Hasanta) {
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
  }

  BengaliT += word;
  updateCache();
}

void MethodFixedLayoutModern::updateCache() {
  std::vector<std::string> v;
  v.push_back(BengaliT.toStdString());
  im_update_suggest(v, "", true);
}

bool MethodFixedLayoutModern::processKey(int key, bool shift, bool altgr, bool shiftaltgr) {
  switch (key) {
    case VC_BACKSPACE:
      if(BengaliT.length() > 0) {
        QString BT = BengaliT.mid(0, BengaliT.length()-1);
        BengaliT = BT;
        updateCache();
        if(BengaliT.length() <= 0) {
          im_reset();
        }
        return true;
      } else {
        return false;
      }
    case VC_ENTER:
      if(BengaliT.length() > 0) {
        im_commit();
        BengaliT = "";
        return false;
      } else {
        return false;
      }
    case VC_KP_ENTER:
      if(BengaliT.length() > 0) {
        im_commit();
        BengaliT = "";
        return false;
      } else {
        return false;
      }
    case VC_SPACE:
      if(BengaliT.length() > 0) {
        im_commit();
        BengaliT = "";
        return false;
        } else {
          return false;
        }
    case VC_UP:
    case VC_LEFT:
    case VC_RIGHT:
    case VC_DOWN:
      if(BengaliT.length() > 0) {
        im_commit();
        BengaliT = "";
        return false;
      } else {
        return false;
      }
    default:
      break;
  }

  QString pressed = parser.getCharForKey(key, shift, altgr, shiftaltgr);
  if(pressed == "") {
    im_commit();
    BengaliT = "";
    return false;
  } else {
    makeWord(pressed);
    return true;
  }
}
