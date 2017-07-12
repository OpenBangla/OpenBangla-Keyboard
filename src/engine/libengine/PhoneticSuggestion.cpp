/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2016 Muhammad Mominul Huque <mominul2082@gmail.com>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/*
 * This suggestion making mechanism is highly inspaired from ibus-avro.
 * For showing respect to ibus-avro project & Mehdi Hasan Khan, I am
 * releasing this code as MPL 2.0 . So this code is now dual licensed
 * under the MPL 2 and the GNU GPL 3 .
 */

#include <QRegularExpression>
#include "PhoneticSuggestion.h"
#include "qlevenshtein.hpp"

PhoneticSuggestion::PhoneticSuggestion() {}

void PhoneticSuggestion::setLayout(QJsonObject lay) {
  parser.setLayout(lay);
}

QMap<QString, QString> PhoneticSuggestion::separatePadding(QString word) {
  QMap<QString, QString> cutted;
  QRegularExpression rgx("(^(?::`|\\.`|[-\\]\\\\~!@#&*()_=+\\[{}'\";<>/?|.,])*?(?=(?:,{2,}))|^(?::`|\\.`|[-\\]\\\\~!@#&*()_=+\\[{}'\";<>/?|.,])*)(.*?(?:,,)*)((?::`|\\.`|[-\\]\\\\~!@#&*()_=+\\[{}'\";<>/?|.,])*$)");
  QRegularExpressionMatch match = rgx.match(word);
  if(match.hasMatch()) {
    cutted["prefix"] = match.captured(1);
    cutted["term"] = match.captured(2);
    cutted["suffix"] = match.captured(3);
  }
  return cutted;
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

void PhoneticSuggestion::appendIfNotContains(QVector<QString> &array, QString item) {
  if (!array.contains(item)) {
    array.append(item);
  }
}

QVector<QString> PhoneticSuggestion::getSuggestion(QString word) {
  QVector<QString> suggestion;
  QVector<QString> dictSuggestion;

  QString phonetic = parser.parse(PadMap["term"]);

  // Add AutoCorrect
  if (autodict.getCorrected(word) != "") {
    // Exact, its a smily
    if (autodict.getCorrected(word) == word) {
      suggestion.append(autodict.getCorrected(word));
    } else {
      dictSuggestion.append(autodict.getCorrected(word));
    }
  } else {
    // The whole word if not present. So search without padding.
    if (autodict.getCorrected(PadMap["term"]) != "") {
      dictSuggestion.append(autodict.getCorrected(PadMap["term"]));
    }
  }

  // Add Dictionary suggestion
  QString dictKey = PadMap["term"].toLower();
  if (dictKey != "") {
    // Get cached results
    QVector<QString> cached = phoneticCache[dictKey];

    if (cached.isEmpty()) {
      dictSuggestion.append(db.find(dictKey).toVector());
      // Update the cache
      if (!dictSuggestion.isEmpty()) {
        phoneticCache[dictKey] = dictSuggestion;
      }
    }
  }


  // Add suffix
  QVector<QString> tempList;
  QString fullWord;
  int len = dictKey.length();

  QVector<QString> rList;
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
            } else {
              if (cacheRightChar == "\u09ce") {
                fullWord = cacheItem.mid(0, cacheItem.length() - 1) + "\u09a4" + suffix;
                tempList.append(fullWord);
              } else if (cacheRightChar == "\u0982") {
                fullWord = cacheItem.mid(0, cacheItem.length() - 1) + "\u0999" + suffix;
                tempList.append(fullWord);
              } else {
                fullWord = cacheItem + suffix;
                tempList.append(fullWord);
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

  // Sort it
  std::sort(rList.begin(), rList.end(), [&] (QString i, QString j) {
    int dist1 = levenshtein_distance(phonetic, i);
    int dist2 = levenshtein_distance(phonetic, j);
    if(dist1 < dist2) {
      return i > j;
    } else if(dist1 > dist2) {
      return i < j;
    } else {
      return i < j;
    }
  });

  QVector<QString> tempSuggestion;

  for(auto& item : rList) {
    appendIfNotContains(tempSuggestion, item);
  }

  // Add Clasic Phonetic
  appendIfNotContains(tempSuggestion, phonetic);

  // Add padding to all
  for (auto& item : tempSuggestion) {
    item = PadMap["prefix"] + item + PadMap["suffix"];
  }

  // Create suggestion
  suggestion.append(tempSuggestion);

  return suggestion;
}

/* TODO: Be sure that it works...
*/
QString PhoneticSuggestion::getPrevSelected() {
  QString selected;
  QString word = PadMap["term"];
  int len = word.length();

  if (cacheMan.getCandidateSelection(PadMap["term"]) != "") {
    selected = cacheMan.getCandidateSelection(PadMap["term"]);
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

            break;
          }
        }
      }
    }
  }

  return PadMap["prefix"] + selected + PadMap["suffix"];
}

void PhoneticSuggestion::saveSelection(QString selected) {
  QMap<QString, QString> mapTerm = separatePadding(selected);
  cacheMan.writeCandidateSelection(PadMap["term"], mapTerm["term"]);
}

QVector<QString> PhoneticSuggestion::Suggest(QString cache) {
  //Seperate begining and trailing padding characters, punctuations etc. from whole word
  PadMap = separatePadding(cache);

  //Convert begining and trailing padding text to phonetic Bangla
  PadMap["prefix"] = parser.parse(PadMap["prefix"]);
  PadMap["suffix"] = parser.parse(PadMap["suffix"]);

  QVector<QString> candidates = getSuggestion(cache);
  if(candidates.isEmpty()) {
    candidates << PadMap["prefix"];
  }


  return candidates;
}
