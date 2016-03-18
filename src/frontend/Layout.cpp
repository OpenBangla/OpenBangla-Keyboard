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

/* Layout handiling code for the frontend. */

#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QByteArray>
#include "Layout.h"
#include "Settings.h"
#include <QDebug>
Layout *gLayout;

Layout::~Layout() {
  // Close the file handler
  fin.close();
}

void Layout::loadLayout(QString path) {
  // Open the given layout file
  fin.setFileName(path);
  fin.open(QIODevice::ReadOnly);
  QByteArray data = fin.readAll();

  // Load Layout(Json) file
  QJsonDocument json(QJsonDocument::fromJson(data));
  lf = json.object();
  // Load it's Description
  loadDesc();

  fin.close();
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
  // Layout Image 0
  lD.image0.clear();
  lD.image0.append(lf.value("info").toObject().value("layout").toObject().value("image0").toString());
  // Layout Image 0
  lD.image1.clear();
  lD.image1.append(lf.value("info").toObject().value("layout").toObject().value("image1").toString());
  // Layout Develper Name
  lD.devName = lf.value("info").toObject().value("layout").toObject().value("developer").toObject().value("name").toString();
  // Layout Developer Comment
  lD.devComment = lf.value("info").toObject().value("layout").toObject().value("developer").toObject().value("comment").toString();
}

LayoutDesc Layout::getDesc() {
  // We have loaded Loaded LayoutDesc earlier, so just return it
  return lD;
}

LayoutList Layout::searchLayoutsEx(QDir dir) {
  LayoutList layoutList;
  QStringList flist = dir.entryList(QStringList("*.json"), QDir::Files);
  for(auto& file : flist) {
    // Actual file path
    QString path = dir.path() + "/" + file;
    // Load the file temporary
    loadLayout(path);
    // Make the internal map and list
    layoutMap[lD.name] = path;
    layoutList << lD.name;
  }
  return layoutList;
}

LayoutList Layout::searchLayouts() {
  LayoutList lst;

  // Clean the internal map and lists
  layoutMap.clear();

  QDir dir;
  dir.setPath(PKGDATADIR "/layouts");
  lst << searchLayoutsEx(dir);
  dir.setPath(QDir::homePath() + "/.OpenBangla-Keyboard/Layouts/");
  lst << searchLayoutsEx(dir);

  return lst;
}

void Layout::setLayout(QString name) {
  // Get the actual path and load the layout
  loadLayout(layoutMap[name]);
  gSettings->setLayoutName(name);
  gSettings->setLayoutPath(layoutMap[name]);
}
