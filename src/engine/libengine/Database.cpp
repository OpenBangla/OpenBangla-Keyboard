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
#include "Database.h"
#include "regexparser.h"
#include "log.h"

Database::Database() {
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(PKGDATADIR "/data/database.db3");

  QStringList table = {"A", "AA", "B", "BH",
                       "C", "CH", "D", "Dd", "Ddh", "Dh",
                       "E", "G", "Gh", "H", "I", "II",
                       "J", "JH", "K", "KH", "Khandatta",
                       "L", "M", "N", "NGA", "NN", "NYA",
                       "O","OI", "OU", "P", "PH",
                       "R", "RR", "RRH", "RRI",
                       "S", "SH", "SS",
                       "T", "TH", "TT", "TTH",
                       "U", "UU", "Y", "Z"};

  if(db.open()) {
    loadTable(table, db);

    loadSuffixTableFromDatabase(db);

    db.close();
  }
}

Database::~Database() {}

void Database::loadTable(QStringList table, QSqlDatabase dbase) {
  QVector<QString> list;

  for (auto& name : table) {
    // Make a SQL statement
    QSqlQuery query = dbase.exec("SELECT * FROM " + name);

    while(query.next()) {
      list.push_back(query.value("Words").toString());
    }

    word_table[name.toLower()] = list;

    // Finish the SQL statement
    query.finish();
  }
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

  QRegularExpression regex(rgx.parse(word));

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
  } else { return QString(""); }
}
