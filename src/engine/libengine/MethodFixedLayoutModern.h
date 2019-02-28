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

#ifndef METHOD_FIXED_LAYOUT_MODERN
#define METHOD_FIXED_LAYOUT_MODERN

#include <QJsonObject>
#include "Layout.h"
#include "FixedLayoutParser.h"

class MethodFixedLayoutModern : public LayoutMth {
  bool handledKey;
  FixedLayoutParser parser;
  QString BengaliT;
  QString marks;
  Suggestion suggest;
public:
  MethodFixedLayoutModern();

  void updateCache();

  void processKeyPress(QString word);

  void insertKar(QString kar);

  void insertReph();

  void internalBackspace();

  void backspace(QString &string, int step);

  // Functions inherited from class LayoutMth
  void setLayout(QJsonObject lay) override;

  Suggestion getSuggestion(int key, bool shift, bool ctrl, bool alt) override;

  IMCommand handleSpecialKey(int key) override;

  Suggestion getCandidates() override;

  bool handledKeyPress() override;

  void candidateCommited(int index) override;

  void updateEngine() override {
    // Nothing to do :)
  }
};

#endif /* end of include guard: METHOD_FIXED_LAYOUT_MODERN */
