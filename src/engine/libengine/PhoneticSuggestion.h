/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2017 Muhammad Mominul Huque <mominul2082@gmail.com>
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
#include "Database.h"
#include "AutoCorrect.h"
#include "cachemanager.h"

struct Cache {
  QString base;
  QString eng;
};

class PhoneticSuggestion {
private:
  PhoneticParser parser;
  AutoCorrect autodict;
  Database db;
  CacheManager cacheMan;

  QMap<QString, QString> PadMap;
  QMap<QString, QStringList> phoneticCache;
  QMap<QString, Cache> tempCache;

  QMap<QString, QString> separatePadding(QString word);
  bool isKar(QString word);
  bool isVowel(QString word);
  QStringList joinSuggestion(QString writtenWord, QString autoCorrect, QStringList dictSuggestion, QString phonetic, QMap<QString, QString> splitWord);
  void appendIfNotContains(QStringList &array, QString item);
  QStringList sortByPhoneticRelevance(QString phonetic, QStringList dictSuggestion);
  QStringList getDictionarySuggestion(QMap<QString, QString> splitWord);
  QString getAutocorrect(QString word, QMap<QString, QString> splitWord);
  QStringList addSuffix(QMap<QString, QString> splitWord);
  void addToTempCache(QString full, QString base, QString eng);

public:
  PhoneticSuggestion();
  void setLayout(QJsonObject lay);

  QString getPrevSelected();
  void saveSelection(QString selected);

  QStringList Suggest(QString word);
  void updateEngine();
};

#endif /* end of include guard: PHONETIC_SUGGESTION_H */
