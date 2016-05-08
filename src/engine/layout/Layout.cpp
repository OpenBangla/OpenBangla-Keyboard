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

/* Core of Layout Management */

#include <QJsonDocument>
#include <QFile>
#include <QByteArray>
#include "Layout.h"
#include "Settings.h"
#include "MethodPhonetic.h"
#include "MethodFixedLayoutModern.h"

Layout *gLayout;

// Available typing methods
MethodPhonetic mPh;
MethodFixedLayoutModern mFm;

Layout::Layout() {
  gSettings = new Settings();
  loadLayout(gSettings->getLayoutPath());
}

Layout::~Layout() {
  // Close the file handler
  fin.close();
  delete gSettings;
}

void Layout::loadLayout(QString path) {
  // Check if we have already a opened file
  if(fin.isOpen()) fin.close();

  // Open the given layout file
  fin.setFileName(path);
  fin.open(QIODevice::ReadOnly);
  QByteArray data = fin.readAll();

  // Load Layout(Json) file
  QJsonDocument json(QJsonDocument::fromJson(data));
  lf = json.object();
  // Load it's Description
  loadDesc();
  // Set typing method
  setMethod();
  // Send changed layout to typing method
  mth->setLayout(sendLayout());
}

void Layout::loadDesc() {
  // Load Layout Description
  // Layout File Type
  QString type = lf.value("info").toObject().value("type").toString();
  if(type == "phonetic") {
    lD.type = Layout_Phonetic;
  } else {
    lD.type = Layout_Fixed;
  }
}

QJsonObject Layout::sendLayout() {
  return lf.value("layout").toObject();
}

void Layout::setMethod() {
  // Check layout type and set methods
  if(lD.type == Layout_Phonetic) {
    // Selected method is phonetic
    mth = &mPh; // Phonetic Method
  } else {
    // Selected method is fixed layout
    mth = &mFm;
  }
}

void Layout::updateWithSettings() {
  // Check if user has changed layout
  if(fin.fileName() != gSettings->getLayoutPath()) {
    loadLayout(gSettings->getLayoutPath());
  }
}

Suggestion Layout::getSuggestion(int key, bool shift, bool ctrl, bool alt) {
  updateWithSettings();
  return mth->getSuggestion(key, shift, ctrl, alt);
}

IMCommand Layout::handleSpecialKey(int key) {
  return mth->handleSpecialKey(key);
}

Suggestion Layout::getCandidates() {
  return mth->getCandidates();
}

bool Layout::handledKeyPress() {
  return mth->handledKeyPress();
}

void Layout::candidateCommited(std::string commited) {
  mth->candidateCommited(commited);
}
