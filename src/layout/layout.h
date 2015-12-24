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

#ifndef LAYOUT_H
#define LAYOUT_H

#include <fstream>
#include <string>
#include "json.hpp"

/* Core of Layout Management */

enum LayoutType {
  Layout_Phonetic,
  Layout_Fixed
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

/* A pure virtual class. Every Typing Method have to implement this.
 */
class LayoutMth {
public:
  virtual void setLayout(nlohmann::json lay) = 0;
  virtual bool processKey(int key, bool shift, bool altgr, bool shiftaltgr)  = 0;
};

class Layout {
  /* Layout Method handler */
  LayoutMth *mth;

  /* File input handler */
  std::ifstream fin;
  /* Layout File */
  nlohmann::json lf;
  /* Layout Descriptor */
  LayoutDesc lD;

  /* Load Layout Description. Used internaly */
  void loadDesc();
  /* Set typing method. Used internaly */
  void setMethod();
public:
  /* Load a layout from given @path */
  void loadLayout(std::string path);

  /* Get Layout Description */
  LayoutDesc getDesc();

  /* Send key event to selected method for further processing.
   * It is usually called from IM Engine
   */
  bool sendKey(int lkey, bool lshift, bool lctrl, bool lalt);
};

/* Global */
extern Layout *gLayout;

#endif /* end of include guard: LAYOUT_H */
