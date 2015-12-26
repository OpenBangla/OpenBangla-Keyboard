/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015 Muhammad Mominul Huque <nahidbinbaten1995@gmail.com>
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
  *  So this code now dual licensed under the MPL 2 and the GNU GPL 3.
  *  If you have any questions about this matter, please send e-mail
  *  at described above.
  *  http://www.gnu.org/licenses/license-list.en.html#MPL-2.0
  */

#ifndef PHONETIC_PARSER_H
#define PHONETIC_PARSER_H

#include <string>
#include "json.hpp"

class PhoneticParser {
private:
  nlohmann::json layout;

  nlohmann::json patterns;
  std::string vowel;
  std::string cons;
  std::string csen;
  int maxPatternLength;

  char smallCap(char letter);
  bool isVowel(char c);
  bool isConsonant(char c);
  bool isPunctuation(char c);
  bool isExact(std::string needle, std::string heystack, int start, int end, bool strnot);
  bool isCaseSensitive(char c);
public:
  ~PhoneticParser();
  void setLayout(nlohmann::json l);
  std::string parse(std::string input);
  std::string fixString(std::string input);
};

#endif /* end of include guard: PHONETIC_PARSER_H */
