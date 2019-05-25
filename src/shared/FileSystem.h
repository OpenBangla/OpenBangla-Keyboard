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

/* We follow XDG Directory Specification for storing user specific data.
   https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
 */
class UserFolders {
  QString path;
  QDir dir;
public:
  UserFolders() {
    path = qEnvironmentVariable("XDG_DATA_HOME", dir.homePath() + "/.local/share") + "/openbangla-keyboard";
    // Create our folder in the user specific data folder
    dir.mkpath(path);
    // Create user specific layouts folder
    dir.mkpath(path + "/layouts/");
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

QString DictionaryPath();

QString SuffixDictPath();

QString RegexDictPath();

QString AutoCorrectFilePath();

#endif /* end of include guard: FILE_SYSTEM_H */
