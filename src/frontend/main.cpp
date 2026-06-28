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
#include "PlatformConfig.h"
#include "Log.h"
#include "FileSystem.h"

#ifdef Q_OS_MACOS
  #include "macOS.h"
#endif

int main(int argc, char *argv[]) {  
  QApplication app(argc, argv);
  gUserFolders = new UserFolders();
  gSettings = new Settings();

  app.setApplicationName("OpenBangla Keyboard");
  app.setApplicationVersion(PROJECT_VERSION);

  QCommandLineParser parser;
  parser.setApplicationDescription("OpenBangla Keyboard");
  parser.addHelpOption();
  parser.addVersionOption();
  QCommandLineOption darkIcon("dark","Enable dark theme support");
  QCommandLineOption startInTray("tray","Start in tray");
  QCommandLineOption setupSystem("setup-system","Setup OpenBangla input source");
  parser.addOption(darkIcon);
  parser.addOption(startInTray);
  parser.addOption(setupSystem);
  parser.process(app);
  
  LOG_INFO("Detected Desktop Environment: %s\n", desktopEnvironmentToString(detectDesktopEnvironment()).toStdString().c_str());

  if(parser.isSet(setupSystem)) {
    setupInputSources();
    return 0;
  }

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
  if (parser.isSet(startInTray))
    app.setProperty("invokedAsTray", parser.isSet(startInTray));
  else if(gSettings->getTopBarVisibility())
    w.show();

  app.setQuitOnLastWindowClosed(false);
  return app.exec();
}
