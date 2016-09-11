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

#ifndef SETTINGS_H
#define SETTINGS_H

class QSettings;
class QPoint;
class QString;

class Settings {
  QSettings *setting;
public:
  Settings();
  ~Settings();

  void setTopBarWindowPosition(QPoint pos);
  QPoint getTopBarWindowPosition();
  void setLayoutViewerWindowPosition(QPoint pos);
  QPoint getLayoutViewerWindowPosition();

  void setLayoutName(QString name);
  QString getLayoutName();
  void setLayoutPath(QString path);
  QString getLayoutPath();

  void setEnterKeyClosesPrevWin(bool b);
  bool getEnterKeyClosesPrevWin();

  void setAutoVowelFormFixed(bool b);
  bool getAutoVowelFormFixed();

  void setAutoChandraPosFixed(bool b);
  bool getAutoChandraPosFixed();

  void setTraditionalKarFixed(bool b);
  bool getTraditionalKarFixed();

  void setNumberPadFixed(bool b);
  bool getNumberPadFixed();

  void setOldReph(bool b);
  bool getOldReph();

  void setCandidateWinHorizontal(bool c);
  bool getCandidateWinHorizontal();
};

/* Global */
extern Settings *gSettings;

#endif /* end of include guard: SETTINGS_H */
