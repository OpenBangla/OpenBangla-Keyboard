/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2018 Muhammad Mominul Huque <mominul2082@gmail.com>
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
#include <QPoint>
#include "Settings.h"
#include "FileSystem.h"

Settings *gSettings;

Settings::Settings() {
  setting = new QSettings("OpenBangla", "Keyboard");
  setting->sync();
}

Settings::~Settings() {
  setting->sync();
  delete setting;
}

void Settings::setTopBarWindowPosition(QPoint pos) {
  setting->setValue("window/positions/TopBar", pos);
  setting->sync();
}

QPoint Settings::getTopBarWindowPosition() {
  setting->sync();
  return setting->value("window/positions/TopBar", QPoint(0, 0)).toPoint();
}

void Settings::setLayoutViewerWindowPosition(QPoint pos) {
  setting->setValue("window/positions/LayoutViewer", pos);
  setting->sync();
}

QPoint Settings::getLayoutViewerWindowPosition() {
  setting->sync();
  return setting->value("window/positions/LayoutViewer").toPoint();
}

void Settings::setLayoutName(QString name) {
  setting->setValue("layout/name", name);
  setting->sync();
}

QString Settings::getLayoutName() {
  setting->sync();
  return setting->value("layout/name", "Avro Phonetic").toString();
}

void Settings::setLayoutPath(QString path) {
  setting->setValue("layout/path", path);
  setting->sync();
}

QString Settings::getLayoutPath() {
  setting->sync();
  return setting->value("layout/path", AvroPhoneticLayoutPath()).toString();
}

void Settings::setEnterKeyClosesPrevWin(bool b) {
  setting->setValue("settings/EnterKeyClosesPrevWin", b);
  setting->sync();
}

bool Settings::getEnterKeyClosesPrevWin() {
  setting->sync();
  return setting->value("settings/EnterKeyClosesPrevWin", false).toBool();
}

void Settings::setAutoVowelFormFixed(bool b) {
  setting->setValue("settings/FixedLayout/AutoVowelForm", b);
  setting->sync();
}

bool Settings::getAutoVowelFormFixed() {
  setting->sync();
  return setting->value("settings/FixedLayout/AutoVowelForm", true).toBool();
}

void Settings::setAutoChandraPosFixed(bool b) {
  setting->setValue("settings/FixedLayout/AutoChandraPos", b);
  setting->sync();
}

bool Settings::getAutoChandraPosFixed() {
  setting->sync();
  return setting->value("settings/FixedLayout/AutoChandraPos", true).toBool();
}

void Settings::setTraditionalKarFixed(bool b) {
  setting->setValue("settings/FixedLayout/TraditionalKar", b);
  setting->sync();
}

bool Settings::getTraditionalKarFixed() {
  setting->sync();
  return setting->value("settings/FixedLayout/TraditionalKar", false).toBool();
}

void Settings::setNumberPadFixed(bool b) {
  setting->setValue("settings/FixedLayout/NumberPad", b);
  setting->sync();
}

bool Settings::getNumberPadFixed() {
  setting->sync();
  return setting->value("settings/FixedLayout/NumberPad", true).toBool();
}

void Settings::setOldReph(bool b) {
  setting->setValue("settings/FixedLayout/OldReph", b);
  setting->sync();
}

bool Settings::getOldReph() {
  setting->sync();
  return setting->value("settings/FixedLayout/OldReph", true).toBool();
}

void Settings::setCandidateWinHorizontal(bool b) {
  setting->setValue("settings/CandidateWin/Horizontal", b);
  setting->sync();
}

bool Settings::getCandidateWinHorizontal() {
  setting->sync();
  return setting->value("settings/CandidateWin/Horizontal", true).toBool();
}

void Settings::setShowCWPhonetic(bool b) {
  setting->setValue("settings/CandidateWin/Phonetic", b);
  setting->sync();
}

bool Settings::getShowCWPhonetic() {
  setting->sync();
  return setting->value("settings/CandidateWin/Phonetic", true).toBool();
}

void Settings::setUpdateCheck(bool b) {
  setting->setValue("settings/UpdateCheck", b);
  setting->sync();
}

bool Settings::getUpdateCheck() {
  setting->sync();
  return setting->value("settings/UpdateCheck", true).toBool();
}

void Settings::setIncludeEnglishPrevWin(bool b) {
  setting->setValue("settings/PreviewWin/IncludeEnglishPhonetic", b);
  setting->sync();
}

bool Settings::getIncludeEnglishPrevWin() {
  setting->sync();
  return setting->value("settings/PreviewWin/IncludeEnglishPhonetic", true).toBool();
}

void Settings::setPreviousUserDataRemains(bool b) {
  setting->setValue("settings/PreviousUserDataRemains", b);
  setting->sync();
}

bool Settings::getPreviousUserDataRemains() {
  setting->sync();
  return setting->value("settings/PreviousUserDataRemains", true).toBool();
}
