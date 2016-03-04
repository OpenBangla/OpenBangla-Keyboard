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

#include <QRegularExpression>
#include "phoneticsuggestion.h"
#include "qlevenshtein.hpp"
#include <QDebug>

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
/*
bool PhoneticSuggestion::isKar(QString word){
  if(word.length() < 1) {
    return false;
  }
  QRegularExpression rgx("^[\\x{09BE}\\x{09BF}\\x{09C0}\\x{09C1}\\x{09C2}\\x{09C3}\\x{09C7}\\x{09C8}\\x{09CB}\\x{09CC}\\x{09C4}]$");
  return word.contains(rgx);
}

bool PhoneticSuggestion::isVowel(QString word) {
  if(word.length() < 1) {
    return false;
  }
  QRegularExpression rgx("^[\\x{0985}\\x{0986}\\x{0987}\\x{0988}\\x{0989}\\x{098A}\\x{098B}\\x{098F}\\x{0990}\\x{0993}\\x{0994}\\x{098C}\\x{09E1}\\x{09BE}\\x{09BF}\\x{09C0}\\x{09C1}\\x{09C2}\\x{09C3}\\x{09C7}\\x{09C8}\\x{09CB}\\x{09CC}]$");
  return word.contains(rgx);
}*/

QVector<QString> PhoneticSuggestion::getSuggestion(QString term) {
  QVector<QString> list;
  QString parsed = parser.parse(term);
  // If we have cached the suggestions before, send them
  list << cacheMan.getTempCache(term);
  if(list.isEmpty()) {
    // Add Auto Correct
    QString autodct = autodict.getCorrected(term);
    if(autodct != "") list.push_back(autodct);

    // Add Dictionary suggestion
    QVector<QString> dictList;
    if(term != "") {
      dictList = db.find(term);
    }
    if(!(dictList.isEmpty())) {
      // Remove the AutoCorrect suggestion if it matches with dictionary suggestion
      if((autodct != "") && (dictList.contains(autodct))) {
        list.removeOne(autodct);
      }
      // Sort Dictionary suggestions using Levenshtien distance algorithm
      std::sort(dictList.begin(), dictList.end(), [&] (QString i, QString j) {
        int dist1 = levenshtein_distance(parsed, i);
        int dist2 = levenshtein_distance(parsed, j);
        if(dist1 < dist2) {
          return i > j;
        } else if(dist1 > dist2) {
          return i < j;
        } else {
          return i < j;
        }
      });
      list << dictList;
    }
    // Save the suggestions temporary
    cacheMan.setTempCache(term, list);
  }
/* TODO: Add Suffix implementation
  // Suggestions with Suffix
  bool alreadySelected = false;
  cacheMan.removeAllBase();
  for(int i = term.length()-1; i > 0; --i) {
    QString suffix = db.banglaForSuffix(term.mid(i).toLower());
    if (suffix != "") {
      QString base = term.mid(0, i);
      QVector<QString> cached = cacheMan.getTempCache(base);
      QString selected;
      if (!alreadySelected) {
        // Base user selection
        selected = cacheMan.getCandidateSelection(base);
      }
      // This should always exist, so it's just a safety check
      if (!cached.isEmpty()) {
        for (auto& item : cached) {
          // Skip AutoCorrect English Entry
          if (base == item) {
            continue;
          }
          QString word;
          // Again saving humanity cause I'm Superman, no I'm not drunk or on weed :D
          int cutPos = item.length() - 1;

          QString itemRMC = item.mid(cutPos);   // RMC is Right Most Character
          QString suffixLMC = suffix.mid(0, 1);      // LMC is Left Most Character
          // BEGIN :: This part was taken from http://d.pr/zTmF
          if (isVowel(itemRMC) && isKar(suffixLMC)) {
            word = item + "\u09df" + suffix;
          }
          else {
            if (itemRMC == "\u09ce") {
              word = item.mid(0, cutPos) + "\u09a4" + suffix;
            }
            else if (itemRMC == "\u0982") {
              word = item.mid(0, cutPos) + "\u0999" + suffix;
            } else {
              word = item + suffix;
            }
          }
          // END

          // Reverse Suffix Caching
          QVector<QString> temp;
          temp << base << item;
          cacheMan.setBase(temp, word);

          // Check that the WORD is not already in the list
          if (!list.contains(word)) {
            // Intelligent Selection
            if (!alreadySelected && (selected != "") && (item == selected)) {
              if (!(cacheMan.getCandidateSelection(term) != "")) {
                cacheMan.writeCandidateSelection(word, term);
              }
              alreadySelected = true;
            }
            list << word;
          }
        }
      }
    }
  }*/

  // Check if the the list already contains the classic phonetic item.
  if(!list.contains(parsed)) {
    list << parsed;
  }
  return list;
}

QString PhoneticSuggestion::getPrevSelected() {
  QString prev = cacheMan.getCandidateSelection(PadMap["term"]);
  return PadMap["prefix"] + prev + PadMap["suffix"];
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

  QVector<QString> candidates = getSuggestion(PadMap["term"]);
  if(!candidates.isEmpty()) {
    // Add padding to all, that we have captured.
    for(auto& candidate : candidates) {
      candidate = PadMap["prefix"] + candidate + PadMap["suffix"];
    }
    // Exact autocorrect
    if((cache != PadMap["term"])) {
      QString autocorrect = autodict.getCorrected(cache);
      if(autocorrect != "") {
        if(!candidates.contains(autocorrect)) {
          candidates.insert(0, autocorrect);
        }
      }
    }
  } else {
    candidates << PadMap["prefix"];
  }


  return candidates;
}
