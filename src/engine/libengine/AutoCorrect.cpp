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
#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include "AutoCorrect.h"
#include "log.h"

AutoCorrect::AutoCorrect() {
  QFile dictFile(PKGDATADIR "/data/autocorrect.json");
  if (!dictFile.open(QIODevice::ReadOnly)) {
    LOG_ERROR("[AutoCorrect]: Error: Couldn't open autocorrect dictionary file!\n");
  }
  QByteArray data = dictFile.readAll();
  QJsonDocument json(QJsonDocument::fromJson(data));

  dict = json.object().value("autocorrect").toObject();
  dictFile.close();

  // Now load user specific AutoCorrect file
  loadUserAutoCorrectFile();
}

void AutoCorrect::loadUserAutoCorrectFile() {
  QFile dictFile(folders.getUserAutoCorrectFile());
  if (!dictFile.open(QIODevice::ReadOnly)) {
    LOG_ERROR("[AutoCorrect]: Error: Couldn't open user specific AutoCorrect file!\n");
  }
  QByteArray data = dictFile.readAll();
  QJsonDocument json(QJsonDocument::fromJson(data));

  usrDict = json.object();
  dictFile.close();
}

QString AutoCorrect::getCorrected(QString word) {
  // Always prefer user edited Autocorrect file
  QJsonValue corrected = usrDict.value(word);

  if(corrected.type() != QJsonValue::Undefined) {
    return corrected.toString();
  } else {
    // Not found in user's AutoCorrect file. So use the
    // default AutoCorrect file.
    corrected = dict.value(word);
    if(corrected.type() != QJsonValue::Undefined) {
      return corrected.toString();
    } else {
      return QString("");
    }
  }
}

QVariantMap AutoCorrect::getEntries() {
  QVariantMap dct = dict.toVariantMap();

  /* Insert user's AutoCorrect entries.
   * If a conflict is found, we prefer user's entry.
   */
  QJsonObject::const_iterator iter = usrDict.constBegin();
  while(iter != usrDict.constEnd()) {
    dct.insert(iter.key(), iter.value());

    ++iter;
  }

  return dct;
}

void AutoCorrect::loadAvroPhonetic() {
  // Now load Avro Phonetic
  QFile layoutFile(PKGDATADIR "/layouts/avrophonetic.json");
  if (!layoutFile.open(QIODevice::ReadOnly)) {
    LOG_ERROR("[AutoCorrect]: Error: Couldn't open Avro Phonetic layout file!\n");
  }

  QByteArray data = layoutFile.readAll();
  QJsonDocument json = QJsonDocument::fromJson(data);
  parser.setLayout(json.object().value("layout").toObject());
  layoutFile.close();
}

QString AutoCorrect::convertBanglish(QString text) {
  return parser.parse(text);
}

void AutoCorrect::setEntry(QString replace, QString with) {
  usrDict.insert(replace, with);
}

void AutoCorrect::saveUserAutoCorrectFile() {
  QFile saveFile(folders.getUserAutoCorrectFile());
  if(!saveFile.open(QIODevice::WriteOnly)) {
    LOG_ERROR("[AutoCorrect:Save]: Error couldn't open save file.\n");
    return;
  }

  QJsonDocument json(usrDict);
  saveFile.write(json.toJson());

  saveFile.close();
}
