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

 #include "layout/layout.h"

 void Layout::loadLayout(std::string path) {
   // Check if we have already a opened file
   if(fin.is_open()) fin.close();

   // Open the given layout file
   fin.open(path, std::ifstream::in);

   // Load Layout(Json) file
   mth->layout << fin;
   // Load it's Description
   loadDesc();
 }

 void Layout::loadDesc() {
   // Load Layout Description
   // Layout File Type
   switch(mth->layout["info"]['type']) {
     case "phonetic":
      lD.type = Layout_Phonetic;
      break;
     case "fixed":
      lD.type = Layout_Fixed;
      break;
   }

   // Layout File Version
   lD.filever = mth->layout["info"]["ver"];
   // Layout Name
   lD.name = mth->layout["info"]["layout_name"];
   // Layout Version
   lD.ver = mth->layout["info"]["layout_ver"];
   // Layout Develper Name
   lD.dev_name = mth->layout["info"]["layout_dev_name"];
   // Layout Developer Comment
   lD.dev_comment = mth->layout["info"]["layout_dev_comment"];
 }

 LayoutDesc Layout::getDesc() {
   // We have loaded Loaded LayoutDesc earlier, so just return it
   return lD;
 }
