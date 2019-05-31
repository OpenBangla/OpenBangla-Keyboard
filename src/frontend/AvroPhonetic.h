/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2016 Muhammad Mominul Huque <mominul2082@gmail.com>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/*
 *  Actually this parser is originated from the parser written by
 *  Rifat Nabi for iAvro under MPL 1.1. It was written in Objective C.
 *  I(Muhammad Mominul Huque) have re-written the parser in C++ for using with
 *  OpenBangla Keyboard.
 *  For showing respect to the Avro project and Rifat Nabi, I am
 *  releasing this code under MPL 2.0.
 *  So this code is now dual licensed under the MPL 2 and the GNU GPL 3.
 *  If you have any questions about this matter, please send e-mail to
 *  me at described above.
 *  http://www.gnu.org/licenses/license-list.en.html#MPL-2.0
 */

#ifndef AVRO_PHONETIC_H
#define AVRO_PHONETIC_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>

class AvroPhonetic {
private:
  QJsonArray patterns;
  QString vowel;
  QString cons;
  QString num;
  QString csen;
  int maxPatternLength;

  bool isVowel(QChar c);

  bool isConsonant(QChar c);

  bool isPunctuation(QChar c);

  bool isNumber(QChar c);

  bool isExact(QString needle, QString heystack, int start, int end, bool strnot);

  bool isCaseSensitive(QChar c);

public:
  AvroPhonetic();
  ~AvroPhonetic();

  QString convert(QString input);

  QString fixString(QString input);
};

#endif /* end of include guard: AVRO_PHONETIC_H */
