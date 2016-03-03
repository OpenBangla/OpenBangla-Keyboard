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
#include <QSettings>
#include <QString>
#include <QPoint>
#include "Settings.h"

Settings *gSettings;

Settings::Settings() {
  setting = new QSettings("OpenBangla", "Keyboard");
  setting->sync();
}

Settings::~Settings() {
  setting->sync();
  delete setting;
}

QString Settings::getLayoutPath() {
  setting->sync();
  return setting->value("layout/path").toString();
}
