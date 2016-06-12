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

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include "cachemanager.h"
#include "log.h"
#include <QDebug>

CacheManager::CacheManager() {
  loadCandidateSelection();
}

void CacheManager::setTempCache(QString key, QVector<QString> suggestions) {
  tempCache[key] = suggestions;
}

QVector<QString> CacheManager::getTempCache(QString key) {
  return tempCache.value(key);
}

void CacheManager::loadCandidateSelection() {
  QFile loadFile(folders.getCandidateSaveFile());
  if(!loadFile.open(QIODevice::ReadOnly)) {
    LOG_ERROR("[CacheManager:Load]: Error couldn't open save file.\n");
    return;
  }
  QByteArray jsonData = loadFile.readAll();

  QJsonDocument json(QJsonDocument::fromJson(jsonData));
  candidateSel = json.object();

  loadFile.close();
}

QString CacheManager::getCandidateSelection(QString word) {
  if(candidateSel.contains(word)) {
    return candidateSel[word].toString();
  } else {
    return QString("");
  }
}

void CacheManager::writeCandidateSelection(QString word, QString sel) {
  candidateSel[word] = sel;
  saveCandidateSelection();
}

void CacheManager::saveCandidateSelection() {
  QFile saveFile(folders.getCandidateSaveFile());
  if(!saveFile.open(QIODevice::WriteOnly)) {
    LOG_ERROR("[CacheManager:Save]: Error couldn't open save file.\n");
    return;
  }

  QJsonDocument json(candidateSel);
  saveFile.write(json.toJson());

  saveFile.close();
}
