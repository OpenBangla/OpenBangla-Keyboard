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

#include <string>
#include "CEngine.h"
#include "Layout.h"

void engine_init() {
  gLayout = new Layout();
}

CSuggestion suggestion_to_c(Suggestion suggest) {
  CSuggestion cs;

  for(int i = 0; i <= MAX_SUGGESTION; i++) {
    cs.candidates[i] = (char *)suggest.candidates[i].c_str();
  }

  cs.auxiliaryText = (char *)suggest.auxiliaryText.c_str();
  cs.prevSelection = suggest.prevSelection;
  cs.showCandidateWin = suggest.showCandidateWin;

  return cs;
}

CSuggestion engine_get_suggestion(int key, bool shift, bool ctrl, bool alt) {
  Suggestion suggest = gLayout->getSuggestion(key, shift, ctrl, alt);
  return suggestion_to_c(suggest);
}

CSuggestion engine_get_candidates() {
  Suggestion suggest = gLayout->getCandidates();
  return suggestion_to_c(suggest);
}

void engine_candidate_commited(char *candidate) {
  std::string str(candidate);
  gLayout->candidateCommited(str);
}

bool engine_key_handled() {
  return gLayout->handledKeyPress();
}

CIMCommand engine_handle_special_key(int key) {
  IMCommand command = gLayout->handleSpecialKey(key);
  CIMCommand cm;
  cm.accepted = command.accepted;
  cm.commit = command.commit;
  cm.needReset = command.needReset;
  cm.needUpdate = command.needUpdate;

  return cm;
}
