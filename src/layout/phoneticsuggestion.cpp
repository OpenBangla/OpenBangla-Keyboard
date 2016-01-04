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

#include "phoneticsuggestion.h"
#include <QDebug>

PhoneticSuggestion::PhoneticSuggestion() {}

void PhoneticSuggestion::setLayout(QJsonObject lay) {
  parser.setLayout(lay);
}

std::vector<std::string> PhoneticSuggestion::Suggest(QString cache) {
  std::vector<std::string> list;

  QString parsed = parser.parse(cache);

  list.clear();
  // Add Auto Correct
  QString autodct = autodict.getCorrected(cache);
  if(autodct != "") list.push_back(autodct.toStdString());
  list.push_back(parsed.toStdString());

  std::vector<std::string> dbb = db.find(cache);
  for(auto& str : dbb) {
    list.push_back(str);
  }
  return list;
}
