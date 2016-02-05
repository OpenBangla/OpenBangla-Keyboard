/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2016 Muhammad Mominul Huque <mominul2082@gmail.com>
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

#ifndef PHONETIC_SUGGESTION_H
#define PHONETIC_SUGGESTION_H

#include <QMap>
#include <QString>
#include <QJsonObject>
#include "phoneticparser.h"
#include "database.h"
#include "autocorrect.h"
#include "cachemanager.h"

class PhoneticSuggestion {
private:
  PhoneticParser parser;
  AutoCorrect autodict;
  Database db;
  CacheManager cacheMan;

  QMap<QString, QString> PadMap;
public:
  PhoneticSuggestion();
  void setLayout(QJsonObject lay);

  QMap<QString, QString> separatePadding(QString word);
  //bool isKar(QString word);
  //bool isVowel(QString word);
  QVector<QString> getSuggestion(QString term);
  QString getPrevSelected();
  void saveSelection(QString selected);

  QVector<QString> Suggest(QString cache);
};

#endif /* end of include guard: PHONETIC_SUGGESTION_H */
