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

#ifndef LAYOUT_H
#define LAYOUT_H

#include <QFile>
#include <QMap>
#include <QString>
#include <QVector>
#include <QJsonObject>

/* Layout handiling code for the frontend */

class QDir;

enum LayoutType {
  Layout_Phonetic,
  Layout_Fixed
};

/* Structure for storing Layout Description */
struct LayoutDesc {
  LayoutType type;
  int fileVer;
  QString name;
  QString ver;
  QByteArray image0;
  QByteArray image1;
  QString devName;
  QString devComment;
};

/* List of available layouts */
typedef QVector<QString> LayoutList;

class Layout {
  /* File input handler */
  QFile fin;
  /* Layout File */
  QJsonObject lf;
  /* Layout Descriptor */
  LayoutDesc lD;
  /* Internal map of available layouts */
  QMap<QString, QString> layoutMap;

  /* Load Layout Description. Used internaly */
  void loadDesc();
  /* Search for layouts in @dir and return the list of them
   * This also resets internal map and lists */
  LayoutList searchLayoutsEx(QDir dir);
  /* Load the layout from given @path */
  void loadLayout(QString path);

public:
  ~Layout();

  /* Get Layout Description */
  LayoutDesc getDesc();

  /* Search for available layouts and return the list of them */
  LayoutList searchLayouts();

  /*  */
  void setLayout(QString name);
};

/* Global */
extern Layout *gLayout;

#endif /* end of include guard: LAYOUT_H */
