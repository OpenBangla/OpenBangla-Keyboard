/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015 Muhammad Mominul Huque <nahidbinbaten1995@gmail.com>
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
#include <QDebug>
#include "autocorrect.h"
#include "log.h"

AutoCorrect::AutoCorrect() {
  QFile dictFile(PKGDATADIR "/data/autocorrect.json");
  if (!dictFile.open(QIODevice::ReadOnly)) {
    LOG_ERROR("[AutoCorrect]: Error: Couldn't open dictionary file\n");
  }
  QByteArray data = dictFile.readAll();
  QJsonDocument json(QJsonDocument::fromJson(data));

  dict = json.object().value("autocorrect").toObject();
  dictFile.close();
}

QString AutoCorrect::getCorrected(QString word) {
  QJsonValue corrected = dict.value(word);
  if(!(corrected.type() == QJsonValue::Undefined)) {
    return corrected.toString();
  } else {
    return QString("");
  }
}
