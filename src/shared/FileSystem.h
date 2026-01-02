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

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <QDir>
#include "Log.h"

/* For Linux we follow XDG Directory Specification for storing user specific data.
   https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html

   On macOS we use ~/Library/Containers/org.openbangla.inputmethod.keyboard/Data/Library/Application Support/org.openbangla.inputmethod.keyboard
   as the user specific data folder and copy data files from the application bundle to this folder on first run.
 */
class UserFolders {
  QString path;
  QDir dir;
public:
  UserFolders() {
      #ifdef Q_OS_LINUX
          setupLinux();
      #elif defined(Q_OS_MACOS)
          setupMacOS();
      #else
          LOG_ERROR("Unsupported OS for UserFolders\n");
      #endif
  }

  void setupLinux();
  void setupMacOS();
  
  QString dataPath() {
    return path;
  }

  QString getCandidateSaveFile() {
    return path + "/phonetic-candidate-selection.json";
  }

  QString getUserLayoutPath() {
    return path + "/layouts/";
  }

  QString getUserAutoCorrectFile() {
    return path + "/autocorrect.json";
  }
};

QString LayoutsFilePath();

QString AvroPhoneticLayoutPath();

QString DatabasePath();

QString DictionaryPath();

QString SuffixDictPath();

QString RegexDictPath();

QString AutoCorrectFilePath();

bool migrateFile(const QString &fileName, const QDir &src, const QDir &dst);

/* Global */
extern UserFolders *gUserFolders;

#endif /* end of include guard: FILE_SYSTEM_H */
