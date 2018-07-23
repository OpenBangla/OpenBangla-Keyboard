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
 *  So this is code now dual licensed under the MPL 2 and the GNU GPL 3.
 *  If you have any questions about this matter, please send e-mail to
 *  me at described above.
 *  http://www.gnu.org/licenses/license-list.en.html#MPL-2.0
 */
#ifndef REGEX_PARSER_H
#define REGEX_PARSER_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class RegexParser {
private:
  QJsonArray patterns;
  QString vowel;
  QString cons;
  QString ign;
  int maxPatternLength;

  bool isVowel(QChar c);

  bool isConsonant(QChar c);

  bool isPunctuation(QChar c);

  bool isExact(QString needle, QString heystack, int start, int end, bool strnot);

  bool isIgnore(QChar c);

  QString cleanString(QString input);

  QString makeRegexCompatible(QString input);

public:
  RegexParser();

  ~RegexParser();

  QString parse(QString input);
};

#endif /* end of include guard: REGEX_PARSER_H */
