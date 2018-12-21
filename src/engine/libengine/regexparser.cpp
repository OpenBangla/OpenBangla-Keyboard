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

#include <QFile>
#include "regexparser.h"
#include "FileSystem.h"
#include "Log.h"

RegexParser::RegexParser() {
  QFile grammarFile(RegexDictPath());
  if (!grammarFile.open(QIODevice::ReadOnly)) {
    LOG_ERROR("[RegexParser]: Error: Couldn't open grammar file\n");
  }
  QByteArray data = grammarFile.readAll();
  QJsonDocument json(QJsonDocument::fromJson(data));

  QJsonObject grammar = json.object();

  patterns = grammar.value("patterns").toArray();
  QString _find = patterns.at(0).toObject().value("find").toString();
  maxPatternLength = _find.length();
  vowel = grammar.value("vowel").toString();
  cons = grammar.value("consonant").toString();
  ign = grammar.value("ignore").toString();

  grammarFile.close();
}

RegexParser::~RegexParser() = default;

QString RegexParser::parse(QString input) {
  // Check
  if (input.length() == 0)
    return input;

  QString fixed = cleanString(input);
  QString output;

  int len = fixed.length();
  for (int cur = 0; cur < len; ++cur) {
    int start = cur, end;
    bool matched = false;

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
              for (auto &&rul : rules) {

                QJsonObject rule = rul.toObject();
                bool replace = true;
                int chk = 0;
                QJsonArray matches = rule.value("matches").toArray();
                for (auto &&matche : matches) {

                  QJsonObject match = matche.toObject();
                  QString value = match.value("value").toString();
                  QString type = match.value("type").toString();
                  QString scope = match.value("scope").toString();
                  bool isNegative = match.value("negative").toBool();

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
                  output += "(্[যবম])?(্?)([ঃঁ]?)";
                  cur = end - 1;
                  matched = true;
                  break;
                }
              }
            }

            if (matched)
              break;

            // Default
            QString rl = pattern.value("replace").toString();
            output += rl;
            output += "(্[যবম])?(্?)([ঃঁ]?)";
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
        if (matched)
          break;
      }
    }

    if (!matched) {
      output += fixed.at(cur);
    }
  }

  return makeRegexCompatible(output);
}

QString RegexParser::makeRegexCompatible(QString input) {
  QString output;
  for (auto &str : input) {
    if (str.unicode() >= 255) {
      output += "\\x{0" + QString::number(str.unicode(), 16).toUpper() + "}";
    } else {
      output += str;
    }
  }
  return QString("^" + output + "$");
}

QString RegexParser::cleanString(QString input) {
  QString fixed;
  for (const auto &c : input) {
    if (!isIgnore(c)) {
      fixed += c.toLower();
    }
  }
  return fixed;
}

bool RegexParser::isVowel(QChar c) {
  return vowel.contains(c, Qt::CaseInsensitive);
}

bool RegexParser::isConsonant(QChar c) {
  return cons.contains(c, Qt::CaseInsensitive);
}

bool RegexParser::isPunctuation(QChar c) {
  return (!(isVowel(c) || isConsonant(c)));
}

bool RegexParser::isExact(QString needle, QString heystack, int start, int end, bool strnot) {
  int len = end - start;
  return ((start >= 0 && end < heystack.length() && (heystack.mid(start, len) == needle)) ^ strnot);
}

bool RegexParser::isIgnore(QChar c) {
  return ign.contains(c, Qt::CaseInsensitive);
}
