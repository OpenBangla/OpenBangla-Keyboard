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

#include "FileSystem.h"

QString LayoutsFilePath() {
    return "/usr/share/openbangla-keyboard/layouts";
}

QString AvroPhoneticLayoutPath() {
    return "/usr/share/openbangla-keyboard/layouts/avrophonetic.json";
}

QString DictionaryPath() {
    return "/usr/share/openbangla-keyboard/data/dictionary.json";
}

QString SuffixDictPath() {
    return "/usr/share/openbangla-keyboard/data/suffix.json";
}

QString RegexDictPath() {
    return "/usr/share/openbangla-keyboard/data/regex.json";
}

QString AutoCorrectFilePath() {
    return "/usr/share/openbangla-keyboard/data/autocorrect.json";
}

QString environmentVariable(const char *varName, const QString &defaultValue)
{
    QByteArray value = qgetenv(varName);
    if (value.isNull())
        return defaultValue;
    return QString::fromLocal8Bit(value);
}
