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

#include <QApplication>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "TopBar.h"
#include "SingleInstance.h"
#include "Settings.h"
#include "FileSystem.h"

int main(int argc, char *argv[]) {  
  QApplication app(argc, argv);
  gSettings = new Settings();
  gUserFolders = new UserFolders();

  app.setApplicationName("OpenBangla Keyboard");
  app.setApplicationVersion(PROJECT_VERSION);

  QCommandLineParser parser;
  parser.setApplicationDescription("OpenBangla Keyboard");
  parser.addHelpOption();
  parser.addVersionOption();
  QCommandLineOption darkIcon("dark","Enable dark theme support");
  QCommandLineOption startInTray("tray","Start in tray");
  parser.addOption(darkIcon);
  parser.addOption(startInTray);
  parser.process(app);

  // Prevent many instances of the app to be launched
  QString name = "com.openbangla.keyboard";
  SingleInstance instance;
  if (instance.hasPrevious(name)) {
    QMessageBox msgBox(QMessageBox::Information,
                       "OpenBangla Keyboard",
                       "OpenBangla Keyboard is already running on this system and\nrunning more than one instance is not allowed.",
                       QMessageBox::Ok);
    msgBox.exec();
    return 0;
  }

  instance.listen(name);

  // Detect Dark Mode
  QPalette palette = app.palette();
  QColor color = palette.color(QPalette::Button);
  QColor darker = QColor::fromRgb(55, 55, 55); // Grayish color used for button in dark mode

  bool darkMode = (color == darker) || parser.isSet(darkIcon);

  TopBar w(darkMode);
  w.show();
  if (parser.isSet(startInTray) || !gSettings->getTopBarVisibility()) {
    w.setVisible(false);
  }
  return app.exec();
}
