/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2016 Muhammad Mominul Huque <mominul2082@gmail.com>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QRegularExpression>
#include "Database.h"

Database::Database() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(PKGDATADIR "/data/database.db3");

    QStringList table = {"A", "AA", "B", "BH",
                         "C", "CH", "D", "Dd", "Ddh", "Dh",
                         "E", "G", "Gh", "H", "I", "II",
                         "J", "JH", "K", "KH", "Khandatta",
                         "L", "M", "N", "NGA", "NN", "NYA",
                         "O", "OI", "OU", "P", "PH",
                         "R", "RR", "RRH", "RRI",
                         "S", "SH", "SS",
                         "T", "TH", "TT", "TTH",
                         "U", "UU", "Y", "Z"};

    prefixTableMap = {{'a', {"a",   "aa",  "e",   "oi",  "o", "nya", "y"}},
                      {'b', {"b",   "bh"}},
                      {'c', {"c",   "ch",  "k"}},
                      {'d', {"d",   "dh",  "dd",  "ddh"}},
                      {'e', {"i",   "ii",  "e",   "y"}},
                      {'f', {"ph"}},
                      {'g', {"g",   "gh",  "j"}},
                      {'h', {"h"}},
                      {'i', {"i",   "ii",  "y"}},
                      {'j', {"j",   "jh",  "z"}},
                      {'k', {"k",   "kh"}},
                      {'l', {"l"}},
                      {'m', {"h",   "m"}},
                      {'n', {"n",   "nya", "nga", "nn"}},
                      {'o', {"a",   "u",   "uu",  "oi",  "o", "ou",  "y"}},
                      {'p', {"p",   "ph"}},
                      {'q', {"k"}},
                      {'r', {"rri", "h",   "r",   "rr",  "rrh"}},
                      {'s', {"s",   "sh",  "ss"}},
                      {'t', {"t",   "th",  "tt",  "tth", "khandatta"}},
                      {'u', {"u",   "uu",  "y"}},
                      {'v', {"bh"}},
                      {'w', {"o"}},
                      {'x', {"e",   "k"}},
                      {'y', {"i",   "y"}},
                      {'z', {"h",   "j",   "jh",  "z"}}
    };

    if (db.open()) {
        loadTable(table, db);

        loadSuffixTableFromDatabase(db);

        db.close();
    }
}

Database::~Database() {}

void Database::loadTable(QStringList table, QSqlDatabase dbase) {
    QVector<QString> list;

    for (auto &name : table) {
        // Make a SQL statement
        QSqlQuery query = dbase.exec("SELECT * FROM " + name);

        while (query.next()) {
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

    while (query.next()) {
        suffix_table[query.value("English").toString()] = query.value("Bangla").toString();
    }
    // Finish the SQLite statement
    query.finish();
}

QStringList Database::find(QString word) {
    if (word != "") {
        QStringList suggestions;
        QChar lmc = word.at(0); // Left Most Character

        QRegularExpression regex(rgx.parse(word));

        for (auto &table : prefixTableMap[lmc]) {
            QVector<QString> tableData = word_table[table];
            for (auto &tmpString : tableData) {
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
        return suffix_table[word];
    } else { return QString(""); }
}
