/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2016 Muhammad Mominul Huque <mominul2082@gmail.com>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
  QMap<QString, QVector<QString>> phoneticCache;

  QMap<QString, QString> separatePadding(QString word);
  bool isKar(QString word);
  bool isVowel(QString word);
  QVector<QString> getSuggestion(QString word);
  void appendIfNotContains(QVector<QString> &array, QString item);
public:
  PhoneticSuggestion();
  void setLayout(QJsonObject lay);

  QString getPrevSelected();
  void saveSelection(QString selected);

  QVector<QString> Suggest(QString cache);
};

#endif /* end of include guard: PHONETIC_SUGGESTION_H */
