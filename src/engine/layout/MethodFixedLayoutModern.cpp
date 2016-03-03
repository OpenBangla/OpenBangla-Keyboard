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

#include "im.h"
#include "MethodFixedLayoutModern.h"

void MethodFixedLayoutModern::setLayout(QJsonObject lay) {
  parser.setLayout(lay);
}

void MethodFixedLayoutModern::makeWord(QString word) {
  BengaliT += word;
  updateCache();
}

void MethodFixedLayoutModern::updateCache() {
  im_update(BengaliT.toStdString());
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
    default:
      break;
  }

  QString pressed = parser.getCharForKey(key, shift, altgr, shiftaltgr);
  if(pressed == "") {
    return false;
  } else {
    makeWord(pressed);
  }
}
