/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2018 Muhammad Mominul Huque <mominul2082@gmail.com>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QFile>
#include <QJsonDocument>
#include <QRegularExpression>
#include "Database.h"
#include "FileSystem.h"
#include "Log.h"

Database::Database() {
  QFile dictFile(DictionaryPath());
  if (!dictFile.open(QIODevice::ReadOnly)) {
    LOG_ERROR("[Database]: Error: Couldn't open dictionary file!\n");
  }

  word_table = QJsonDocument::fromJson(dictFile.readAll()).object();

  dictFile.close();
  dictFile.setFileName(SuffixDictPath());
  dictFile.open(QIODevice::ReadOnly);

  suffix_table = QJsonDocument::fromJson(dictFile.readAll()).object();
  dictFile.close();

  prefixTableMap = {{'a', {"a", "aa", "e", "oi", "o", "nya", "y"}},
                    {'b', {"b", "bh"}},
                    {'c', {"c", "ch", "k"}},
                    {'d', {"d", "dh", "dd", "ddh"}},
                    {'e', {"i", "ii", "e", "y"}},
                    {'f', {"ph"}},
                    {'g', {"g", "gh", "j"}},
                    {'h', {"h"}},
                    {'i', {"i", "ii", "y"}},
                    {'j', {"j", "jh", "z"}},
                    {'k', {"k", "kh"}},
                    {'l', {"l"}},
                    {'m', {"h", "m"}},
                    {'n', {"n", "nya", "nga", "nn"}},
                    {'o', {"a", "u", "uu", "oi", "o", "ou", "y"}},
                    {'p', {"p", "ph"}},
                    {'q', {"k"}},
                    {'r', {"rri", "h", "r", "rr", "rrh"}},
                    {'s', {"s", "sh", "ss"}},
                    {'t', {"t", "th", "tt", "tth", "khandatta"}},
                    {'u', {"u", "uu", "y"}},
                    {'v', {"bh"}},
                    {'w', {"o"}},
                    {'x', {"e", "k"}},
                    {'y', {"i", "y"}},
                    {'z', {"h", "j", "jh", "z"}}
  };
}

Database::~Database() = default;

QStringList Database::find(QString word) {
  if (word != "") {
    QStringList suggestions;
    QChar lmc = word.at(0); // Left Most Character

    QRegularExpression regex(rgx.parse(word));

    for (auto &table : prefixTableMap[lmc]) {
      QJsonArray tableData = word_table[table].toArray();
      for (auto tmpValue : tableData) {
        QString tmpString = tmpValue.toString();
        if (tmpString.contains(regex)) {
          suggestions.push_back(tmpString);
        }
      }
    }
    suggestions.removeDuplicates();
    return suggestions;
  } else {
    return {""};
  }
}

QString Database::banglaForSuffix(QString word) {
  if (suffix_table.contains(word)) {
    return suffix_table[word].toString();
  } else {
    return QString("");
  }
}
