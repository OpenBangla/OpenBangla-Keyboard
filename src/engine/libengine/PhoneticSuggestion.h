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
#include <QRegularExpression>
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

  QRegularExpression rgxPadding;
  QRegularExpression rgxKar;
  QRegularExpression rgxVowel;

  QString padBegin, padMiddle, padEnd;
  QMap<QString, QStringList> phoneticCache;
  QMap<QString, Cache> tempCache;
  QStringList prevSuggestion;

  void separatePadding(QString word);

  bool isKar(QString word);

  bool isVowel(QString word);

  QStringList joinSuggestion(QString writtenWord, QString autoCorrect, QStringList dictSuggestion, QString phonetic);

  void appendIfNotContains(QStringList &array, QString item);

  QStringList getDictionarySuggestion();

  QString getAutocorrect(QString word);

  QStringList addSuffix();

  void addToTempCache(QString full, QString base, QString eng);

public:
  PhoneticSuggestion();

  void setLayout(QJsonObject lay);

  QString getPrevSelected();

  void saveSelection(int index);

  QStringList Suggest(QString word);

  void updateEngine();
};

#endif /* end of include guard: PHONETIC_SUGGESTION_H */
