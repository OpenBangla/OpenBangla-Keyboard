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

/* Core of Layout Management */

#include "layout.h"

Layout *gLayout;

void Layout::loadLayout(std::string path) {
  // Check if we have already a opened file
  if(fin.is_open()) fin.close();

  // Open the given layout file
  fin.open(path, std::ifstream::in);

  // Load Layout(Json) file
  lf << fin;
  // Load it's Description
  loadDesc();
  // Set typing method
  setMethod();
  // Send changed layout to typing method
  mth->setLayout(lf);
}

void Layout::loadDesc() {
  // Load Layout Description
  // Layout File Type
  std::string type = lf["info"]["type"];
  if(type == "phonetic") {
    lD.type = Layout_Phonetic;
  } else {
    lD.type = Layout_Fixed;
  }

  // Get values
  int FileVer = lf["info"]["version"];
  std::string Name = lf["info"]["layout"]["name"];
  std::string Ver = lf["info"]["layout"]["version"];
  std::string DevName = lf["info"]["layout"]["developer"]["name"];
  std::string DevComment = lf["info"]["layout"]["developer"]["comment"];
  // Layout File Version
  lD.fileVer = FileVer;
  // Layout Name
  lD.name = Name;
  // Layout Version
  lD.ver = Ver;
  // Layout Develper Name
  lD.devName = DevName;
  // Layout Developer Comment
  lD.devComment = DevComment;
}

void Layout::setMethod() {
  // Check layout type and set methods
  if(lD.type == Layout_Phonetic) {
    // Selected method is phonetic
    mth = mPh; // Phonetic Method
  } else {
    // Selected method is fixed layout
    // TODO: implement
  }
}

bool Layout::sendKey(int lkey, bool lshift, bool lctrl, bool lalt) {
  // Set modifiers
  bool laltgr, lshiftaltgr;

  // Don't catch Ctrl without Shift
  if(lctrl || !lshift) { return false; }

  if(lctrl && lalt) { laltgr = true; } else { laltgr = false; }
  if(lshift && laltgr) { lshiftaltgr = true; } else { lshiftaltgr = false; }

  return mth->processKey(lkey, lshift, laltgr, lshiftaltgr);
}

LayoutDesc Layout::getDesc() {
  // We have loaded Loaded LayoutDesc earlier, so just return it
  return lD;
}
