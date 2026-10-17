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

/* Layout handling code for the frontend. */

#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include "Layout.h"
#include "Settings.h"
#include "FileSystem.h"

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
  QString name = lf.value("info").toObject().value("layout").toObject().value("name").toString();
  if (type == "transliteration") {
    lD.type = (name == "Khipro") ? Layout_Khipro : Layout_Phonetic;
  } else {
    lD.type = Layout_Fixed;
  }

  // Get values
  // Layout File Version
  lD.fileVer = lf.value("info").toObject().value("version").toInt();
  // Layout Name
  lD.name = name;
  // Layout Version
  lD.ver = lf.value("info").toObject().value("layout").toObject().value("version").toString();
  // Layout Image
  // v3 phonetic layouts carry a single instructional image in "image"; fall back
  // to the legacy "image0" so already-installed v2 phonetic layouts still show it.
  lD.image.clear();
  QJsonObject layoutInfo = lf.value("info").toObject().value("layout").toObject();
  if(!layoutInfo.value("image").isUndefined()) {
    lD.image = layoutInfo.value("image").toString().toUtf8();
  } else if(!layoutInfo.value("image0").isUndefined()) {
    lD.image = layoutInfo.value("image0").toString().toUtf8();
  }
  // Layout Develper Name
  lD.devName = lf.value("info").toObject().value("layout").toObject().value("developer").toObject().value(
      "name").toString();
  // Layout Developer Comment
  lD.devComment = lf.value("info").toObject().value("layout").toObject().value("developer").toObject().value(
      "comment").toString();
}

LayoutDesc Layout::getDesc() {
  // We have loaded Loaded LayoutDesc earlier, so just return it
  return lD;
}

QMap<QString, QString> Layout::getLayoutMap() {
  QMap<QString, QString> map;
  // Only fixed layouts have a per-key mapping to draw a keyboard from.
  if (lD.type != Layout_Fixed) {
    return map;
  }
  // The whole layout file is still available in `lf`; the top level "layout"
  // object holds flat Key_<name>_Normal / Key_<name>_AltGr -> glyph entries.
  QJsonObject layout = lf.value("layout").toObject();
  for (auto it = layout.constBegin(); it != layout.constEnd(); ++it) {
    map.insert(it.key(), it.value().toString());
  }
  return map;
}

LayoutList Layout::searchLayoutsEx(QDir dir) {
  LayoutList layoutList;
  QStringList flist = dir.entryList(QStringList("*.json"), QDir::Files);
  for (auto &file : flist) {
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

#ifndef Q_OS_MACOS
  // Search in system layouts folder
  // On macOS we don't have a system layouts folder, all layouts are stored in user folder
  // so we skip this part
  dir.setPath(LayoutsFilePath());
  lst << searchLayoutsEx(dir);
#endif

  dir.setPath(gUserFolders->getUserLayoutPath());
  if (dir.exists()) {
    lst << searchLayoutsEx(dir);
  } else {
    return lst;
  }

  return lst;
}

void Layout::setLayout(QString name) {
  // Get the actual path and load the layout
  loadLayout(layoutMap[name]);
  gSettings->setLayoutName(name);
  if(lD.type == Layout_Phonetic) {
    // Hardcoded Avro Phonetic setting.
    gSettings->setLayoutPath("avro_phonetic");
  } else if(lD.type == Layout_Khipro) {
    // Hardcoded Khipro setting. The transliteration is handled by riti.
    gSettings->setLayoutPath("khipro");
  } else {
    gSettings->setLayoutPath(layoutMap[name]);
  }
}
