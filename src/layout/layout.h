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

#include <fstream>
#include <string>
#include "3rdparty/json.hpp"

/* Core of Layout Management */

enum LayoutType {
  Layout_Phonetic,
  Layout_Fixed;
};

/* Structure for storing Layout Description */
struct LayoutDesc {
  LayoutType type;
  int fileVer;
  std::string name;
  std::string ver;
  std::string devName;
  std::string devComment;
};

class LowLayout {
protected:
  nlohmann::json layout;
public:
  std::string getCharForKey(int key, bool shift, bool altgr, bool shift_altgr);
  virtual bool sendKey(int key, bool shift, bool ctrl, bool alt);
}

class Layout : public LowLayout {
  LowLayout *mth;
  /* File input handler */
  std::ifstream fin;
  LayoutDesc lD;

  /* Load Layout Description. Used internaly */
  void loadDesc();
public:
  /* Load a layout from given @path */
  void loadLayout(std::string path);
  /* Get Layout Description */
  LayoutDesc getDesc();
  /* Send key event to selected method for further processing.
   * It is usually called from IM Engine
   */
  bool sendKey(int key, bool shift, bool ctrl, bool alt);
};

/* Global */
extern Layout *gLayout;
