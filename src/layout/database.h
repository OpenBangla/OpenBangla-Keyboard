/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015 Muhammad Mominul Huque <nahidbinbaten1995@gmail.com>
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
public:
  Database();
  ~Database();

  void loadTableWithName(QString name, QSqlDatabase dbase);
  void loadSuffixTableFromDatabase(QSqlDatabase dbase);

  std::vector<std::string> find(QString word);
  std::string banglaForSuffix(QString word);
};

#endif /* end of include guard: DATABASE_H */
