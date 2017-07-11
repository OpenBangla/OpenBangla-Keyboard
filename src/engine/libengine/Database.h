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

#include <QtSql>
#include <QString>
#include <QMap>
#include <QVector>

class Database {
  QMap<QString, QVector<QString>> word_table;
  QMap<QString, QString> suffix_table;
  QMap<char, QStringList> prefixTableMap;
public:
  Database();
  ~Database();

  void loadTable(QStringList table, QSqlDatabase dbase);
  void loadSuffixTableFromDatabase(QSqlDatabase dbase);

  QVector<QString> find(QString word);
  QString banglaForSuffix(QString word);
};

#endif /* end of include guard: DATABASE_H */
