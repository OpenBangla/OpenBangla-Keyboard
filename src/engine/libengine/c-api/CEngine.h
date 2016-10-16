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

#ifndef CENGINE_H
#define CENGINE_H

#include "stdbool.h"
#include "keycode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SUGGESTION 9

typedef struct {
  /* Key accepted */
  bool accepted;
  /* Commit current suggestion(candidate) */
  bool commit;
  /* IM needs to update candidates */
  bool needUpdate;
  /* IM needs to reset */
  bool needReset;
} CIMCommand;

typedef struct {
  char *candidates[MAX_SUGGESTION];
  char *auxiliaryText;
  bool showCandidateWin;
  int prevSelection;
} CSuggestion;

void engine_init();
CSuggestion engine_get_suggestion(int key, bool shift, bool ctrl, bool alt);
CSuggestion engine_get_candidates();
void engine_candidate_commited(char *candidate);
bool engine_key_handled();
CIMCommand engine_handle_special_key(int key);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: CENGINE_H */
