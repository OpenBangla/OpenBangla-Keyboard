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

#include <QCoreApplication>
#include "FileSystem.h"
#include "Log.h"

UserFolders *gUserFolders = nullptr;

void UserFolders::setupLinux() {
    path = qEnvironmentVariable("XDG_DATA_HOME", dir.homePath() + "/.local/share") + "/openbangla-keyboard";
    // Create our folder in the user specific data folder
    dir.mkpath(path);
    // Create user specific layouts folder
    dir.mkpath(path + "/layouts/");
}

void UserFolders::setupMacOS() {
    path = dir.homePath() + "/Library/Containers/org.openbangla.inputmethod.keyboard/Data/Library/Application Support/org.openbangla.inputmethod.keyboard";
    // Create our folder in the user specific data folder
    bool ret = dir.mkpath(path);

    // Create user specific layouts folder
    dir.mkpath(path + "/layouts/");

    LOG_DEBUG("OpenBangla IME directory present: %d\n", ret);

    QString resourcePath = QCoreApplication::applicationDirPath() + "/../Resources/";
    QDir resourceDir =  QDir(resourcePath);

    LOG_DEBUG("OpenBangla GUI path: %s\n", resourceDir.absolutePath().toUtf8().constData());

    QDir layoutsDir = QDir(resourceDir.filePath("layouts"));
    QDir userLayoutsDir = QDir(path + "/layouts/");

    QStringList files = layoutsDir.entryList(QDir::Files);

    for (const QString &file : files) {
        // Migrate data files to user specific folder on first run
        bool ret = migrateFile(file, layoutsDir, userLayoutsDir);
        LOG_DEBUG("Migrating layout %s: %d\n", file.toStdString().c_str(), ret);
    }
}

QString LayoutsFilePath() {
#ifdef Q_OS_MACOS
    return gUserFolders->dataPath() + "/layouts/";
#else
    return PROJECT_DATADIR "/layouts";
#endif
}

QString AvroPhoneticLayoutPath() {
#ifdef Q_OS_MACOS
    return gUserFolders->dataPath() + "/layouts/avrophonetic.json";
#else
    return PROJECT_DATADIR "/layouts/avrophonetic.json";
#endif
}

QString DatabasePath() {
    return PROJECT_DATADIR "/data";
}

QString DictionaryPath() {
    return PROJECT_DATADIR "/data/dictionary.json";
}

QString SuffixDictPath() {
    return PROJECT_DATADIR "/data/suffix.json";
}

QString RegexDictPath() {
    return PROJECT_DATADIR "/data/regex.json";
}

QString AutoCorrectFilePath() {
#ifdef Q_OS_MACOS
    return QCoreApplication::applicationDirPath() + "/../Resources/data/autocorrect.json";
#else
    return PROJECT_DATADIR "/data/autocorrect.json";
#endif
}

/// Copy the `fileName` from `src` to `dst`.
/// This function overwrites if the file already exists in the destination.
bool migrateFile(const QString &fileName, const QDir &src, const QDir &dst) {
    QString srcFile = src.filePath(fileName);
    QString dstFile = dst.filePath(fileName);

    if(QFile::exists(srcFile) && QFile::exists(dstFile)) {
        QFile::remove(dstFile);
    }

    return QFile::copy(srcFile, dstFile);
}
