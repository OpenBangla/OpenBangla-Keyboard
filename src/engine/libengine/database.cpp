/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2016 Muhammad Mominul Huque <mominul2082@gmail.com>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QRegularExpression>
#include <QStringList>
#include "database.h"
#include "regexparser.h"
#include "log.h"

Database::Database() {
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(PKGDATADIR "/data/database.db3");
  if(db.open()) {
    loadTableWithName("A", db);
    loadTableWithName("AA", db);
    loadTableWithName("B", db);
    loadTableWithName("BH", db);
    loadTableWithName("C", db);
    loadTableWithName("CH", db);
    loadTableWithName("D", db);
    loadTableWithName("Dd", db);
    loadTableWithName("Ddh", db);
    loadTableWithName("Dh", db);
    loadTableWithName("E", db);
    loadTableWithName("G", db);
    loadTableWithName("Gh", db);
    loadTableWithName("H", db);
    loadTableWithName("I", db);
    loadTableWithName("II", db);
    loadTableWithName("J", db);
    loadTableWithName("JH", db);
    loadTableWithName("K", db);
    loadTableWithName("KH", db);
    loadTableWithName("Khandatta", db);
    loadTableWithName("L", db);
    loadTableWithName("M", db);
    loadTableWithName("N", db);
    loadTableWithName("NGA", db);
    loadTableWithName("NN", db);
    loadTableWithName("NYA", db);
    loadTableWithName("O", db);
    loadTableWithName("OI", db);
    loadTableWithName("OU", db);
    loadTableWithName("P", db);
    loadTableWithName("PH", db);
    loadTableWithName("R", db);
    loadTableWithName("RR", db);
    loadTableWithName("RRH", db);
    loadTableWithName("RRI", db);
    loadTableWithName("S", db);
    loadTableWithName("SH", db);
    loadTableWithName("SS", db);
    loadTableWithName("T", db);
    loadTableWithName("TH", db);
    loadTableWithName("TT", db);
    loadTableWithName("TTH", db);
    loadTableWithName("U", db);
    loadTableWithName("UU", db);
    loadTableWithName("Y", db);
    loadTableWithName("Z", db);

    loadSuffixTableFromDatabase(db);

    db.close();
  }
}

Database::~Database() {}

void Database::loadTableWithName(QString name, QSqlDatabase dbase) {
  QVector<QString> list;

  // Make a SQL statement
  QSqlQuery query = dbase.exec("SELECT * FROM " + name);

  while(query.next()) {
    list.push_back(query.value("Words").toString());
  }

  word_table[name.toLower()] = list;

  // Finish the SQL statement
  query.finish();
}

void Database::loadSuffixTableFromDatabase(QSqlDatabase dbase) {
  // Make a SQL statement
  QSqlQuery query = dbase.exec("SELECT * FROM Suffix");

  while(query.next()) {
    suffix_table[query.value("English").toString()] = query.value("Bangla").toString();
  }
  // Finish the SQLite statement
  query.finish();
}

QVector<QString> Database::find(QString word) {
  RegexParser rgx;
  QStringList tablelist;
  QVector<QString> suggestions;
  char lmc = word.toStdString().at(0); // Left Most Character

  QRegularExpression regex(rgx.parse(word), QRegularExpression::OptimizeOnFirstUsageOption);

  switch (lmc) {
    case 'a':
      tablelist << "a" << "aa" << "e" << "oi" << "o" << "nya" << "y";
      break;
    case 'b':
      tablelist << "b" << "bh";
      break;
    case 'c':
      tablelist << "c" << "ch" << "k";
      break;
    case 'd':
      tablelist << "d" << "dh" << "dd" << "ddh";
      break;
    case 'e':
      tablelist << "i" << "ii" << "e" << "y";
      break;
    case 'f':
      tablelist << "ph";
      break;
    case 'g':
      tablelist << "g" << "gh" << "j";
      break;
    case 'h':
      tablelist << "h";
      break;
    case 'i':
      tablelist << "i" << "ii" << "y";
      break;
    case 'j':
      tablelist << "j" << "jh" << "z";
      break;
    case 'k':
      tablelist << "k" << "kh";
      break;
    case 'l':
      tablelist << "l";
      break;
    case 'm':
      tablelist << "h" << "m";
      break;
    case 'n':
      tablelist << "n" << "nya" << "nga" << "nn";
      break;
    case 'o':
      tablelist << "a" << "u" << "uu" << "oi" << "o" << "ou" << "y";
      break;
    case 'p':
      tablelist << "p" << "ph";
      break;
    case 'q':
      tablelist << "k";
      break;
    case 'r':
      tablelist << "rri" << "h" << "r" << "rr" << "rrh";
      break;
    case 's':
      tablelist << "s" << "sh" << "ss";
      break;
    case 't':
      tablelist << "t" << "th" << "tt" << "tth" << "khandatta";
      break;
    case 'u':
      tablelist << "u" << "uu" << "y";
      break;
    case 'v':
      tablelist << "bh";
      break;
    case 'w':
      tablelist << "o";
      break;
    case 'x':
      tablelist << "e" << "k";
      break;
    case 'y':
      tablelist << "i" << "y";
      break;
    case 'z':
      tablelist << "h" << "j" << "jh" << "z";
      break;
    default:
      break;
  }

  for(auto& table : tablelist) {
    QVector<QString> tableData = word_table[table];
    for(auto& tmpString : tableData) {
      if(tmpString.contains(regex)) {
        suggestions.push_back(tmpString);
      }
    }
  }
  return suggestions;
}

QString Database::banglaForSuffix(QString word) {
  if(suffix_table.contains(word)) {
    return suffix_table[word];
  } else { QString(""); }
}
