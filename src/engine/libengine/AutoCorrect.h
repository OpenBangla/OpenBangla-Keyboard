/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2018 Muhammad Mominul Huque <mominul2082@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef AUTO_CORRECT_H
#define AUTO_CORRECT_H

#include <QString>
#include <QJsonObject>
#include <QMap>
#include <QVariant>
#include "phoneticparser.h"
#include "FileSystem.h"

class AutoCorrect {
  QJsonObject dict;
  QJsonObject usrDict;
  PhoneticParser parser;
  UserFolders folders;
public:
  AutoCorrect();

  QString getCorrected(QString word);

  /* Get AutoCorrect entries */
  QVariantMap getEntries();

  /* Load user AutoCorrect file */
  void loadUserAutoCorrectFile();

  /* Load Avro Phonetic to convert Banglish to Bangla */
  void loadAvroPhonetic();

  /* Convert Banglish to Avro Phonetic */
  QString convertBanglish(QString text);

  /* Add an entry into users's AutoCorrect dictionary */
  void setEntry(QString replace, QString with);

  /* Save user specific AutoCorrect file */
  void saveUserAutoCorrectFile();
};

#endif /* end of include guard: AUTO_CORRECT_H */
