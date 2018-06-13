/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2017-2018 Muhammad Mominul Huque <mominul2082@gmail.com>
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
#include "Settings.h"
#include "qlevenshtein.hpp"

PhoneticSuggestion::PhoneticSuggestion() {
  // Set patterns and optimize them
  rgxPadding.setPattern("(^(?::`|\\.`|[-\\]\\\\~!@#&*()_=+\\[{}'\";<>/?|.,])*?(?=(?:,{2,}))|^(?::`|\\.`|[-\\]\\\\~!@#&*()_=+\\[{}'\";<>/?|.,])*)(.*?(?:,,)*)((?::`|\\.`|[-\\]\\\\~!@#&*()_=+\\[{}'\";<>/?|.,])*$)");
  rgxKar.setPattern("^[\u09be\u09bf\u09c0\u09c1\u09c2\u09c3\u09c7\u09c8\u09cb\u09cc\u09c4]$");
  rgxVowel.setPattern("^[\u0985\u0986\u0987\u0988\u0989\u098a\u098b\u098f\u0990\u0993\u0994\u098c\u09e1\u09be\u09bf\u09c0\u09c1\u09c2\u09c3\u09c7\u09c8\u09cb\u09cc]$");
  rgxPadding.optimize();
  rgxKar.optimize();
  rgxVowel.optimize();
}

void PhoneticSuggestion::setLayout(QJsonObject lay) {
  parser.setLayout(lay);
}

QStringList PhoneticSuggestion::getDictionarySuggestion() {
  QStringList words;

  QString key = padMiddle.toLower();

  if (phoneticCache.contains(key)) {
    words = phoneticCache[key];
  } else {
    words = db.find(key);
  }

  return words;
}

QString PhoneticSuggestion::getAutocorrect(QString word) {
  QString corrected;

  QString autoCorrect = autodict.getCorrected(parser.fixString(word));

  if (autoCorrect != "") {
    if (autoCorrect == word) {
      corrected = word;
    } else {
      corrected = parser.parse(autoCorrect);
    }
  } else {
    QString withCorrection = autodict.getCorrected(padMiddle);
    if (withCorrection != "") {
      corrected = parser.parse(withCorrection);
    }
  }

  return corrected;
}

void PhoneticSuggestion::separatePadding(QString word) {
  QRegularExpressionMatch match = rgxPadding.match(word);
  if(match.hasMatch()) {
    padBegin = match.captured(1);
    padMiddle = match.captured(2);
    padEnd = match.captured(3);
  }
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

  return word.left(1).contains(rgxKar);
}

bool PhoneticSuggestion::isVowel(QString word) {
  if (word.length() < 1) {
    return false;
  }

  return word.left(1).contains(rgxVowel);
}

void PhoneticSuggestion::appendIfNotContains(QStringList &array, QString item) {
  if (!array.contains(item)) {
    array.append(item);
  }
}

void PhoneticSuggestion::addToTempCache(QString full, QString base, QString eng) {
  if (!tempCache.contains(full)) {
    tempCache[full] = {base, eng};
  }
}

QStringList PhoneticSuggestion::addSuffix() {
  QStringList tempList;
  QString fullWord;
  QString dictKey = padMiddle.toLower();
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
  QString word = padMiddle;
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

  return padBegin + selected + padEnd;
}

QStringList PhoneticSuggestion::joinSuggestion(QString writtenWord, QString autoCorrect, QStringList dictSuggestion, QString phonetic) {
  QStringList words;

  if (autoCorrect != "") {
    words.append(autoCorrect);
    if (autoCorrect != writtenWord) {
      dictSuggestion.append(autoCorrect);
    }
  }

  if (phoneticCache[padMiddle.toLower()].isEmpty()) {
    if (!dictSuggestion.isEmpty()) {
      phoneticCache[padMiddle.toLower()] = dictSuggestion;
    }
  }

  QStringList dictSuggestionWithSuffix = addSuffix();

  for (auto& word : dictSuggestionWithSuffix) {
    appendIfNotContains(words, word);
  }

  words = sortByPhoneticRelevance(phonetic, words);

  appendIfNotContains(words, phonetic);

  prevSuggestion << words;

  for (auto& word : words) {
    // smiley rule
    if (autoCorrect == writtenWord) {
      if (autoCorrect != word) {
        word = padBegin + word + padEnd;
      }
    } else {
      word = padBegin + word + padEnd;
    }
  }

  return words;
}

void PhoneticSuggestion::saveSelection(int index) {
  cacheMan.writeCandidateSelection(padMiddle, prevSuggestion[index]);
}

QStringList PhoneticSuggestion::Suggest(QString word) {
  QStringList suggestion;
  separatePadding(word);
  prevSuggestion.clear();

  padBegin = parser.parse(padBegin);
  padEnd = parser.parse(padEnd);

  QString phonetic = parser.parse(padMiddle);

  if(!gSettings->getShowCWPhonetic()) {
    // Return only phonetic suggestion
    suggestion.append(padBegin + phonetic + padEnd);
  } else {
    QStringList dictSuggestion = getDictionarySuggestion();
    QString autoCorrect = getAutocorrect(word);

    suggestion = joinSuggestion(word, autoCorrect, dictSuggestion, phonetic);
  }

  return suggestion;
}

void PhoneticSuggestion::updateEngine() {
  autodict.loadUserAutoCorrectFile();
}
