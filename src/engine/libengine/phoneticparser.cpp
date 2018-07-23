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

#include "phoneticparser.h"

void PhoneticParser::setLayout(QJsonObject l) {
  layout = l;
  patterns = layout.value("patterns").toArray();
  QString _find = patterns.at(0).toObject().value("find").toString();
  maxPatternLength = _find.length();
  vowel = layout.value("vowel").toString();
  cons = layout.value("consonant").toString();
  num = layout.value("number").toString();
  csen = layout.value("casesensitive").toString();
}

PhoneticParser::~PhoneticParser() {}

QString PhoneticParser::parse(QString input) {
  QString fixed = fixString(input);
  QString output;

  int len = fixed.length();
  for (int cur = 0; cur < len; ++cur) {
    int start = cur, end;
    bool matched = false;

    int chunkLen;
    for (int chunkLen = maxPatternLength; chunkLen > 0; --chunkLen) {
      end = start + chunkLen;
      if (end <= len) {
        QString chunk = fixed.mid(start, chunkLen);

        // Binary Search
        int left = 0, right = patterns.size() - 1, mid;
        while (right >= left) {
          mid = (right + left) / 2;
          QJsonObject pattern = patterns.at(mid).toObject();
          QString find = pattern.value("find").toString();
          if (find == chunk) {
            QJsonArray rules = pattern.value("rules").toArray();
            if (!(rules.isEmpty())) {
              for (QJsonArray::iterator r = rules.begin(); r != rules.end(); ++r) {
                QJsonValue rul = *r;
                QJsonObject rule = rul.toObject();
                bool replace = true;
                int chk = 0;
                QJsonArray matches = rule.value("matches").toArray();
                for (QJsonArray::iterator m = matches.begin(); m != matches.end(); ++m) {
                  QJsonValue mch = *m;
                  QJsonObject match = mch.toObject();

                  QJsonValue jvalue = match.value("value");
                  QString value;
                  QString type = match.value("type").toString();
                  QString scope = match.value("scope").toString();
                  bool isNegative = false;

                  // Handle Negative
                  if ((scope.at(0) == QLatin1Char('!'))) {
                    isNegative = true;
                    scope = scope.mid(1);
                  }

                  // Handle empty value
                  if (!(jvalue.type() == QJsonValue::Undefined))
                    value = jvalue.toString();

                  if (type == "suffix") {
                    chk = end;
                  }
                    // Prefix
                  else {
                    chk = start - 1;
                  }

                  // Beginning
                  if (scope == "punctuation") {
                    if (
                        !(
                            (chk < 0 && (type == "prefix")) ||
                                (chk >= len && (type == "suffix")) ||
                                isPunctuation(fixed.at(chk))
                        ) ^ isNegative
                        ) {
                      replace = false;
                      break;
                    }
                  }
                    // Vowel
                  else if (scope == "vowel") {
                    if (
                        !(
                            (
                                (chk >= 0 && (type == "prefix")) ||
                                    (chk < len && (type == "suffix"))
                            ) &&
                                isVowel(fixed.at(chk))
                        ) ^ isNegative
                        ) {
                      replace = false;
                      break;
                    }
                  }
                    // Consonant
                  else if (scope == "consonant") {
                    if (
                        !(
                            (
                                (chk >= 0 && (type == "prefix")) ||
                                    (chk < len && (type == "suffix"))
                            ) &&
                                isConsonant(fixed.at(chk))
                        ) ^ isNegative
                        ) {
                      replace = false;
                      break;
                    }
                  }
                    // Number
                  else if (scope == "number") {
                    if (
                        !(
                            (
                                (chk >= 0 && (type == "prefix")) ||
                                    (chk < len && (type == "suffix"))
                            ) &&
                                isNumber(fixed.at(chk))
                        ) ^ isNegative
                        ) {
                      replace = false;
                      break;
                    }
                  }
                    // Exact
                  else if (scope == "exact") {
                    int s, e;
                    if (type == "suffix") {
                      s = end;
                      e = end + value.length();
                    }
                      // Prefix
                    else {
                      s = start - value.length();
                      e = start;
                    }
                    if (!(isExact(value, fixed, s, e, isNegative))) {
                      replace = false;
                      break;
                    }
                  }
                }

                if (replace) {
                  QString rl = rule.value("replace").toString();
                  output += rl;
                  cur = end - 1;
                  matched = true;
                  break;
                }
              }
            }

            if (matched == true)
              break;

            // Default
            QString rl = pattern.value("replace").toString();
            output += rl;
            cur = end - 1;
            matched = true;
            break;
          } else if (find.length() > chunk.length() ||
              (find.length() == chunk.length() && find.compare(chunk, Qt::CaseSensitive) < 0)) {
            left = mid + 1;
          } else {
            right = mid - 1;
          }
        }
        if (matched == true)
          break;
      }
    }

    if (!matched) {
      output += fixed.at(cur);
    }
  }

  return output;
}

QString PhoneticParser::fixString(QString input) {
  QString fixed;
  for (const auto &c : input) {
    if (isCaseSensitive(c)) {
      fixed += c;
    } else {
      fixed += c.toLower();
    }
  }
  return fixed;
}

bool PhoneticParser::isVowel(QChar c) {
  return vowel.contains(c, Qt::CaseInsensitive);
}

bool PhoneticParser::isConsonant(QChar c) {
  return cons.contains(c, Qt::CaseInsensitive);
}

bool PhoneticParser::isPunctuation(QChar c) {
  return (!(isVowel(c) || isConsonant(c)));
}

bool PhoneticParser::isNumber(QChar c) {
  return num.contains(c, Qt::CaseInsensitive);
}

bool PhoneticParser::isExact(QString needle, QString heystack, int start, int end, bool strnot) {
  int len = end - start;
  return ((start >= 0 && end < heystack.length() && (heystack.mid(start, len) == needle)) ^ strnot);
}

bool PhoneticParser::isCaseSensitive(QChar c) {
  return csen.contains(c, Qt::CaseInsensitive);
}
