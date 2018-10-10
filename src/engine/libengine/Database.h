/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2016 Muhammad Mominul Huque <mominul2082@gmail.com>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DATABASE_H
#define DATABASE_H

#include <QJsonObject>
#include <QString>
#include <QMap>
#include "regexparser.h"

class Database {
  QJsonObject word_table;
  QJsonObject suffix_table;
  QMap<QChar, QStringList> prefixTableMap;
  RegexParser rgx;
public:
  Database();

  ~Database();

  QStringList find(QString word);

  QString banglaForSuffix(QString word);
};

#endif /* end of include guard: DATABASE_H */
