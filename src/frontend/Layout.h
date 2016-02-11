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
#include <QString>
#include <QVector>
#include <QJsonObject>

/* Core of Layout Management */

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
  QString devName;
  QString devComment;
};

class Layout {
  /* File input handler */
  QFile fin;
  /* Layout File */
  QJsonObject lf;
  /* Layout Descriptor */
  LayoutDesc lD;

  /* Load Layout Description. Used internaly */
  void loadDesc();

public:
  ~Layout();

  /* Load a layout from given @path */
  void loadLayout(QString path);

  QJsonObject sendLayout();

  /* Get Layout Description */
  LayoutDesc getDesc();
};

/* Global */
extern Layout *gLayout;

#endif /* end of include guard: LAYOUT_H */
