/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2016-2018 Muhammad Mominul Huque <mominul2082@gmail.com>
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
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QJsonObject>
#include "LayoutConverter.h"

ConversionResult LayoutConverter::convertLayout(QString path) {
  return convertAvroLayout(path);
}

QString LayoutConverter::unescapeXML(QString escaped) {
  escaped.replace("&amp;", "&");
  escaped.replace("&lt;", "<");
  escaped.replace("&gt;", ">");
  escaped.replace("&apos;", "'");
  escaped.replace("&quot;", "\"");
  return escaped;
}

ConversionResult LayoutConverter::convertAvroLayout(QString path) {
  QFile xmlFile(path);
  if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return OpenError;
  }
  QByteArray xmlData = xmlFile.readAll();
  QXmlStreamReader *xmlReader = new QXmlStreamReader(xmlData);
  QJsonObject layoutDev, infoLayout, info, keys;

  while (!xmlReader->atEnd() && !xmlReader->hasError()) {
    xmlReader->readNext();
    if (xmlReader->isStartDocument()) {
      continue;
    }

    if (xmlReader->isStartElement()) {
      QStringRef name = xmlReader->name();
      if (name == "Layout" || name == "KeyData") {
        continue;
      }

      QString data = xmlReader->readElementText().trimmed();

      // Check layout version, we only support Avro Keyboard 5 layout.
      if (name == "AvroKeyboardVersion") {
        if (data != "5") {
          xmlReader->clear();
          xmlFile.close();
          delete xmlReader;
          return UnsupportedLayout;
        }
      }

      if (!name.contains("Key") && !name.contains("Num")) {
        // Meta data conversion
        if (name == "LayoutName") {
          infoLayout["name"] = data;
        } else if (name == "LayoutVersion") {
          infoLayout["version"] = data;
        } else if (name == "DeveloperName") {
          layoutDev["name"] = data;
        } else if (name == "DeveloperComment") {
          layoutDev["comment"] = unescapeXML(data);
        } else if (name == "ImageNormalShift") {
          infoLayout["image0"] = data.simplified().replace(" ", "");
        } else if (name == "ImageAltGrShift") {
          infoLayout["image1"] = data.simplified().replace(" ", "");
        }
      } else {
        // Key conversion
        QString key = name.toString();
        if (key.contains("OEM1"))
          key.replace("OEM1", "Semicolon");
        if (key.contains("OEM2"))
          key.replace("OEM2", "Slash");
        if (key.contains("OEM3"))
          key.replace("OEM3", "BackQuote");
        if (key.contains("OEM4"))
          key.replace("OEM4", "OpenBracket");
        if (key.contains("OEM5"))
          key.replace("OEM5", "BackSlash");
        if (key.contains("OEM6"))
          key.replace("OEM6", "CloseBracket");
        if (key.contains("OEM7"))
          key.replace("OEM7", "Quote");
        if (key.contains("MINUS"))
          key.replace("MINUS", "Minus");
        if (key.contains("PLUS"))
          key.replace("PLUS", "Equals");
        if (key.contains("PERIOD"))
          key.replace("PERIOD", "Period");
        if (key.contains("COMMA"))
          key.replace("COMMA", "Comma");

        keys[key] = unescapeXML(data);
      }
    }
  }

  info["type"] = "fixed";
  info["version"] = "1";
  infoLayout["developer"] = layoutDev;
  info["layout"] = infoLayout;

  QJsonObject layout;
  layout["info"] = info;
  layout["layout"] = keys;

  xmlReader->clear();
  xmlFile.close();
  delete xmlReader;

  QFileInfo fileInfo(xmlFile);
  QString savePath = folders.getUserLayoutPath() + fileInfo.baseName() + ".json";
  return saveLayout(layout, savePath);
}

ConversionResult LayoutConverter::saveLayout(QJsonObject obj, QString path) {
  QFile saveFile(path);
  if (!saveFile.open(QIODevice::WriteOnly)) {
    return SaveError;
  }

  QJsonDocument json(obj);
  saveFile.write(json.toJson());
  saveFile.close();

  return Ok;
}
