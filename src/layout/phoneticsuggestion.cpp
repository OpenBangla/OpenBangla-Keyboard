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

#include <fstream>
#include "phoneticsuggestion.h"

PhoneticSuggestion::PhoneticSuggestion() {
  std::ifstream fin;
  // Load Auto Correct Dictionary
  fin.open(PKGDATADIR "/data/autocorrect.json", std::ifstream::in);
  autodict << fin;
  fin.close();
}

std::string PhoneticSuggestion::getAutoCorrect(std::string word) {
  std::string fixed = parser.fixString(word);
  // Find the exact matching
  try {
    return autodict["autocorrect"].at(fixed);
  } catch (std::out_of_range) {
    // Not found
    return (std::string)"";
  }
}

void PhoneticSuggestion::setLayout(nlohmann::json lay) {
  parser.setLayout(lay);
}

std::vector<std::string> PhoneticSuggestion::Suggest(std::string cache) {
  std::vector<std::string> list;

  list.clear();
  // Add Auto Correct
  if(getAutoCorrect(cache) != "") list.push_back(getAutoCorrect(cache));
  list.push_back(parser.parse(cache));

  std::vector<std::string> dbb = db.find(cache);
  for(auto& str : dbb) {
    list.push_back(str);
  }
  return list;
}
