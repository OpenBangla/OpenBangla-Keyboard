/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2017 Muhammad Mominul Huque <mominul2082@gmail.com>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/*
 * This suggestion making mechanism is highly inspired from ibus-avro.
 * For showing respect to ibus-avro project & Mehdi Hasan Khan, I am
 * releasing this code as MPL 2.0 . So this code is now dual licensed
 * under the MPL 2 and the GNU GPL 3 .
 */

#include <QRegularExpression>
#include "PhoneticSuggestion.h"
#include "qlevenshtein.hpp"
#include <QDebug>

PhoneticSuggestion::PhoneticSuggestion() {}

void PhoneticSuggestion::setLayout(QJsonObject lay) {
  parser.setLayout(lay);
}

QStringList PhoneticSuggestion::getDictionarySuggestion(QMap<QString, QString> splitWord) {
  QStringList words;

  QString key = splitWord["middle"].toLower();

  if (phoneticCache.contains(key)) {
    words = phoneticCache[key];
  } else {
    words = db.find(key);
  }

  return words;
}

QMap<QString, QString> PhoneticSuggestion::getAutocorrect(QString word, QMap<QString, QString> splitWord) {
  QMap<QString, QString> corrected;

  QString autoCorrect = autodict.getCorrected(word);

  if (autoCorrect != "") {
    if (autoCorrect == word) {
      corrected["corrected"] = word;
      corrected["exact"] = "true";
    } else {
      corrected["corrected"] = autoCorrect;
      corrected["exact"] = "false";
    }
  } else {
    QString withCorrection = autodict.getCorrected(splitWord["middle"]);
    if (withCorrection != "") {
      corrected["corrected"] = withCorrection;
      corrected["exact"] = "false";
    }
  }

  return corrected;
}

QMap<QString, QString> PhoneticSuggestion::separatePadding(QString word) {
  QMap<QString, QString> cutted;
  QRegularExpression rgx("(^(?::`|\\.`|[-\\]\\\\~!@#&*()_=+\\[{}'\";<>/?|.,])*?(?=(?:,{2,}))|^(?::`|\\.`|[-\\]\\\\~!@#&*()_=+\\[{}'\";<>/?|.,])*)(.*?(?:,,)*)((?::`|\\.`|[-\\]\\\\~!@#&*()_=+\\[{}'\";<>/?|.,])*$)");
  QRegularExpressionMatch match = rgx.match(word);
  if(match.hasMatch()) {
    cutted["begin"] = match.captured(1);
    cutted["middle"] = match.captured(2);
    cutted["end"] = match.captured(3);
  }
  return cutted;
}

QStringList PhoneticSuggestion::sortByPhoneticRelevance(QString phonetic, QStringList dictSuggestion) {
  std::sort(dictSuggestion.begin(), dictSuggestion.end(), [&] (QString i, QString j) {
    int dist1 = levenshtein_distance(phonetic, i);
    int dist2 = levenshtein_distance(phonetic, j);
    if(dist1 < dist2) {
      return true;
    } else if(dist1 > dist2) {
      return false;
    } else {
      return true;
    }
  });

  return dictSuggestion;
}

bool PhoneticSuggestion::isKar(QString word) {
  if (word.length() < 1) {
    return false;
  }

  return word.left(1).contains(QRegularExpression("^[\u09be\u09bf\u09c0\u09c1\u09c2\u09c3\u09c7\u09c8\u09cb\u09cc\u09c4]$"));
}

bool PhoneticSuggestion::isVowel(QString word) {
  if (word.length() < 1) {
    return false;
  }

  return word.left(1).contains(QRegularExpression("^[\u0985\u0986\u0987\u0988\u0989\u098a\u098b\u098f\u0990\u0993\u0994\u098c\u09e1\u09be\u09bf\u09c0\u09c1\u09c2\u09c3\u09c7\u09c8\u09cb\u09cc]$"));
}

void PhoneticSuggestion::appendIfNotContains(QStringList &array, QString item) {
  if (!array.contains(item)) {
    array.append(item);
  }
}

void PhoneticSuggestion::addToTempCache(QString full, QString base, QString eng) {
  if (!tempCache.contains(full)) {
    tempCache[full].base = base;
    tempCache[full].eng = eng;
  }
}

QStringList PhoneticSuggestion::addSuffix(QMap<QString, QString> splitWord) {
  QStringList tempList;
  QString fullWord;
  QString dictKey = splitWord["middle"].toLower();
  int len = dictKey.length();

  QStringList rList;
  if (!phoneticCache[dictKey].isEmpty()) {
    rList = phoneticCache[dictKey];
  }

  if (len >= 2) {
    for (int i = 1; i <= len; i++) {
      QString testSuffix = dictKey.mid(i, len);

      QString suffix = db.banglaForSuffix(testSuffix);
      if(suffix != "") {
        QString key = dictKey.mid(0, dictKey.length() - testSuffix.length());
        if (!phoneticCache[key].isEmpty()) {
          for (auto& cacheItem : phoneticCache[key]) {
            QString cacheRightChar = cacheItem.right(1);
            QString suffixLeftChar = suffix.left(1);
            if (isVowel(cacheRightChar) && isKar(suffixLeftChar)) {
              fullWord = cacheItem + "\u09DF" + suffix;
              tempList.append(fullWord);
              addToTempCache(fullWord, cacheItem, key);
            } else {
              if (cacheRightChar == "\u09ce") {
                fullWord = cacheItem.mid(0, cacheItem.length() - 1) + "\u09a4" + suffix;
                tempList.append(fullWord);
                addToTempCache(fullWord, cacheItem, key);
              } else if (cacheRightChar == "\u0982") {
                fullWord = cacheItem.mid(0, cacheItem.length() - 1) + "\u0999" + suffix;
                tempList.append(fullWord);
              } else {
                fullWord = cacheItem + suffix;
                tempList.append(fullWord);
                addToTempCache(fullWord, cacheItem, key);
              }
            }
          }

          for (auto& item : tempList) {
            rList.append(item);
          }
        }
      }
    }
  }

  return rList;
}

QString PhoneticSuggestion::getPrevSelected() {
  QString selected;
  QString word = PadMap["middle"];
  int len = word.length();

  if (cacheMan.getCandidateSelection(word) != "") {
    selected = cacheMan.getCandidateSelection(word);
  } else {
    //Full word was not found, try checking without suffix
    if (len >= 2) {
      for (int j = 1; j < len; j++) {
        QString testSuffix = word.right(j).toLower();

        QString suffix = db.banglaForSuffix(testSuffix);
        if (suffix != "") {
          QString key = word.mid(0, word.length() - testSuffix.length());

          if (cacheMan.getCandidateSelection(key) != "") {
            QString keyWord = cacheMan.getCandidateSelection(key);
            QString kwRightChar = keyWord.right(1);
            QString suffixLeftChar = suffix.left(1);

            if (isVowel(kwRightChar) && isKar(suffixLeftChar)) {
              selected = keyWord + "\u09df" + suffix;
            } else {
              if (kwRightChar == "\u09ce") {
                selected = keyWord.mid(0, keyWord.length() -1) + "\u09a4" + suffix;
              } else if (kwRightChar == "\u0982") {
                selected = keyWord.mid(0, keyWord.length() -1) + "\u0999" + suffix;
              } else {
                selected = keyWord + suffix;
              }
            }
            cacheMan.writeCandidateSelection(word, selected);
            break;
          }
        }
      }
    }
  }

  return PadMap["begin"] + selected + PadMap["end"];
}

QStringList PhoneticSuggestion::joinSuggestion(QMap<QString, QString> autoCorrect, QStringList dictSuggestion, QString phonetic, QMap<QString, QString> splitWord) {
  QStringList words;

  if (autoCorrect["corrected"] != "") {
    words.append(autoCorrect["corrected"]);
    if (autoCorrect["exact"] == "false") {
      dictSuggestion.append(autoCorrect["corrected"]);
    }
  }

  if (phoneticCache[splitWord["middle"].toLower()].isEmpty()) {
    if (!dictSuggestion.isEmpty()) {
      phoneticCache[splitWord["middle"].toLower()] = dictSuggestion;
    }
  }

  QStringList dictSuggestionWithSuffix = addSuffix(splitWord);

  for (auto& word : dictSuggestionWithSuffix) {
    appendIfNotContains(words, word);
  }

  words = sortByPhoneticRelevance(phonetic, words);

  appendIfNotContains(words, phonetic);

  for (auto& word : words) {
    if (autoCorrect["exact"] == "true") {
      if (autoCorrect["corrected"] != word) {
        word = splitWord["begin"] + word + splitWord["end"];
      }
    } else {
      word = splitWord["begin"] + word + splitWord["end"];
    }
  }

  return words;
}

void PhoneticSuggestion::saveSelection(QString selected) {
  if (tempCache.contains(selected)) {
    QString base = tempCache[selected].base;
    QString eng = tempCache[selected].eng;
    if(cacheMan.getCandidateSelection(eng) == "") {
      cacheMan.writeCandidateSelection(eng, base);
    }
  }
}

QStringList PhoneticSuggestion::Suggest(QString word) {
  QMap<QString, QString> splitWord = separatePadding(word);
  PadMap = splitWord;

  splitWord["begin"] = parser.parse(splitWord["begin"]);
  splitWord["end"] = parser.parse(splitWord["end"]);

  QString phonetic = parser.parse(splitWord["middle"]);
  QStringList dictSuggestion = getDictionarySuggestion(splitWord);
  QMap<QString, QString> autoCorrect = getAutocorrect(word, splitWord);

  QStringList suggestion = joinSuggestion(autoCorrect, dictSuggestion, phonetic, splitWord);

  return suggestion;
}
