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

#ifndef METHOD_PHONETIC_H
#define METHOD_PHONETIC_H

#include <QJsonObject>
#include "Layout.h"
#include "PhoneticSuggestion.h"

class MethodPhonetic : public LayoutMth {
  bool handledKey;
  bool changedCandidateSelection = false;
  int prevSelected;
  PhoneticSuggestion suggest;
  QStringList list;
  QString EnglishT;
  Suggestion suggested;

  Suggestion createSuggestion();

  std::vector<std::string> toStdVector(QStringList vec);

public:
  // Functions inherited from class LayoutMth
  void setLayout(QJsonObject lay);

  Suggestion getSuggestion(int key, bool shift, bool ctrl, bool alt) override;

  IMCommand handleSpecialKey(int key) override;

  Suggestion getCandidates() override;

  bool handledKeyPress() override;

  void candidateCommited(int index) override;

  void updateEngine() override;
};

#endif /* end of include guard: METHOD_PHONETIC_H */
