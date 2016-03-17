/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2016 Muhammad Mominul Huque <mominul2082@gmail.com>
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

#include <QFileInfo>
#include "LayoutConverter.h"

void LayoutConverter::convertLayout(QString path) {
  convertAvroLayout(path);
}

QString LayoutConverter::UnescapeXML(QString escaped) {
  escaped.replace("&amp;", "&");
  escaped.replace("&lt;", "<");
  escaped.replace("&gt;", ">");
  escaped.replace("&apos;", "'");
  escaped.replace("&quot;", "\"");
  return escaped;
}

void LayoutConverter::convertAvroLayout(QString path) {
  // Open the to-be converted
  conversionFile.setFileName(path);
  conversionFile.open(QIODevice::ReadOnly);
  data = conversionFile.readAll();

  QDomDocument avrolayout;
  avrolayout.setContent(data);
  layoutRoot = avrolayout.documentElement();

  convertAvroInfo();
  convertAvroKeydata();

  /**/
  QFileInfo info(conversionFile);
  QString savePath = QString(QDir::homePath() + "/.OpenBangla-Keyboard/Layouts/%1").arg(info.baseName() + ".json");

  std::ofstream ofs;
  ofs.open(savePath.toStdString(), std::ofstream::out);
  ofs << std::setw(4) << jsonFile;

  conversionFile.close();
  ofs.close();
}

void LayoutConverter::convertAvroInfo() {
  jsonFile["info"]["type"] = "fixed";
  jsonFile["info"]["version"] = "1";
  jsonFile["info"]["layout"]["name"] = layoutRoot.firstChild().nextSiblingElement("LayoutName").text().toStdString();
  jsonFile["info"]["layout"]["version"] = layoutRoot.firstChild().nextSiblingElement("LayoutVersion").text().toStdString();
  jsonFile["info"]["layout"]["developer"]["name"] = layoutRoot.firstChild().nextSiblingElement("DeveloperName").text().toStdString();
  jsonFile["info"]["layout"]["developer"]["comment"] = UnescapeXML(layoutRoot.firstChild().nextSiblingElement("DeveloperComment").text()).toStdString();
  // Images
  QString image0 = layoutRoot.firstChild().nextSiblingElement("ImageNormalShift").text();
  QString image1 = layoutRoot.firstChild().nextSiblingElement("ImageAltGrShift").text();
  /* This removes many whitespaces and carriage returns('\r\n') thats are not needed */
  jsonFile["info"]["layout"]["image0"] = image0.simplified().replace( " ", "" ).toStdString();
  jsonFile["info"]["layout"]["image1"] = image1.simplified().replace( " ", "" ).toStdString();
}

void LayoutConverter::convertAvroKeydata() {
  QDomElement keydata;
  keydata = layoutRoot.firstChild().nextSiblingElement("KeyData");
  for(QDomNode n = keydata.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    QString key = n.nodeName();
    /* Naming conversions */
    if(key.contains("OEM1")) key.replace("OEM1", "Semicolon");
    if(key.contains("OEM2")) key.replace("OEM2", "Slash");
    if(key.contains("OEM3")) key.replace("OEM3", "BackQuote");
    if(key.contains("OEM4")) key.replace("OEM4", "OpenBracket");
    if(key.contains("OEM5")) key.replace("OEM5", "BackBracket");
    if(key.contains("OEM6")) key.replace("OEM6", "CloseBracket");
    if(key.contains("OEM7")) key.replace("OEM7", "Quote");
    if(key.contains("MINUS")) key.replace("MINUS", "Minus");
    if(key.contains("PLUS")) key.replace("PLUS", "Equals");
    if(key.contains("PERIOD")) key.replace("PERIOD", "Period");
    if(key.contains("COMMA")) key.replace("COMMA", "Comma");

    jsonFile["layout"][key.toStdString()] = UnescapeXML(n.toElement().text()).toStdString();
  }
}
