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
#include "qlevenshtein.hpp"
#include <QDebug>

PhoneticSuggestion::PhoneticSuggestion() {}

void PhoneticSuggestion::setLayout(QJsonObject lay) {
  parser.setLayout(lay);
}

std::vector<std::string> PhoneticSuggestion::toStdVector(QVector<QString> vec) {
  std::vector<std::string> v;
  for(auto& str : vec) {
    v.push_back(str.toStdString());
  }
  return v;
}

std::vector<std::string> PhoneticSuggestion::Suggest(QString cache) {
  QVector<QString> list;
  list.clear();

  QString parsed = parser.parse(cache);

  // Add Auto Correct
  QString autodct = autodict.getCorrected(cache);
  if(autodct != "") list.push_back(autodct);

  // Add Dictionary suggestion
  QVector<QString> dictList = db.find(cache);
  // Remove the AutoCorrect suggestion if it matches with dictionary suggestion
  if((autodct != "") && (dictList.contains(autodct))) {
    list.removeOne(autodct);
  }
  // Sort Dictionary suggestions using Levenshtien distance algorithm
  std::sort(dictList.begin(), dictList.end(), [&] (QString i, QString j) {
    int dist1 = levenshtein_distance(parsed, i);
    int dist2 = levenshtein_distance(parsed, j);
    if(dist1 < dist2) {
      return i > j;
    } else if(dist1 > dist2) {
      return i < j;
    } else {
      return i < j;
    }
  });
  list << dictList;

  if(!(list.contains(parsed))) {
    list.push_back(parsed);
  }
  return toStdVector(list);
}
