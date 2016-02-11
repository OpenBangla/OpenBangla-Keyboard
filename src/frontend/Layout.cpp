/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2016 Muhammad Mominul Huque <mominul2082@gmail.com>
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

/* Core of Layout Management */

#include <QJsonDocument>
#include <QFile>
#include <QByteArray>
#include "layout.h"

Layout *gLayout;

Layout::~Layout() {
  // Close the file handler
  fin.close();
}

void Layout::loadLayout(QString path) {
  // Check if we have already a opened file
  if(fin.isOpen()) fin.close();

  // Open the given layout file
  fin.setFileName(path);
  fin.open(QIODevice::ReadOnly);
  QByteArray data = fin.readAll();

  // Load Layout(Json) file
  QJsonDocument json(QJsonDocument::fromJson(data));
  lf = json.object();
  // Load it's Description
  loadDesc();
  // Set typing method
  setMethod();
  // Send changed layout to typing method
  mth->setLayout(sendLayout());
}

void Layout::loadDesc() {
  // Load Layout Description
  // Layout File Type
  QString type = lf.value("info").toObject().value("type").toString();
  if(type == "phonetic") {
    lD.type = Layout_Phonetic;
  } else {
    lD.type = Layout_Fixed;
  }

  // Get values
  // Layout File Version
  lD.fileVer = lf.value("info").toObject().value("version").toInt();
  // Layout Name
  lD.name = lf.value("info").toObject().value("layout").toObject().value("name").toString();
  // Layout Version
  lD.ver = lf.value("info").toObject().value("layout").toObject().value("version").toString();
  // Layout Develper Name
  lD.devName = lf.value("info").toObject().value("layout").toObject().value("developer").toObject().value("name").toString();
  // Layout Developer Comment
  lD.devComment = lf.value("info").toObject().value("layout").toObject().value("developer").toObject().value("comment").toString();
}

LayoutDesc Layout::getDesc() {
  // We have loaded Loaded LayoutDesc earlier, so just return it
  return lD;
}
