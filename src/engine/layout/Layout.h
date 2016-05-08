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

#ifndef LAYOUT_H
#define LAYOUT_H

#include <QFile>
#include <QString>
#include <QJsonObject>

/* Core of Layout Management */

/* It is the Suggestion structure that contains all the informations(data)
 * are needed by the IM to show suggestions(candidates) to the client.
 * @candidates a std::vector that contains all the candidates
 * @auxiliaryText a std::string that contains the auxiliary text that is going to be shown in the preview window.
 * @showCandidateWin a bool that controls whether preview window will be shown.
 * @prevSelection a int that contains the index number of the suggestion that was chosen by the user.
 *
 * @func isEmpty()
 * @return bool
 * @description Checks is there is any candidate in @candidates
 */
struct Suggestion {
  std::vector<std::string> candidates;
  std::string auxiliaryText;
  bool showCandidateWin;
  int prevSelection;

  bool isEmpty() {
    return candidates.empty();
  }
};

struct IMCommand {
  /* Key accepted */
  bool accepted = false;
  /* Commit current suggestion(candidate) */
  bool commit = false;
  /* IM needs to update candidates */
  bool needUpdate = false;
  /* IM needs to reset */
  bool needReset = false;
};

/* Commit current suggestion(candidate) */
#define commandCommit   (1 << 1)
/* The key has been accepted. So don't send it to the client */
#define commandAccept   (1 << 2)
/* The key has been rejected. So send it to the client */
#define commandIgnore   (1 << 3)
/* IM needs to reset */
#define commandIMReset  (1 << 4)
/* IM needs to update candidates */
#define commandIMUpdate (1 << 5)

enum LayoutType {
  Layout_Phonetic,
  Layout_Fixed
};

/* Structure for storing Layout Description */
struct LayoutDesc {
  LayoutType type;
};

/* A pure virtual class. Every Typing Method have to implement this.
 */
class LayoutMth {
public:
  virtual void setLayout(QJsonObject lay) = 0;
  /* Generates suggestions on the flow */
  virtual Suggestion getSuggestion(int key, bool shift, bool ctrl, bool alt)  = 0;
  /* Handle special keys such as Enter, Backspace, Space etc keys */
  virtual IMCommand handleSpecialKey(int key) = 0;
  /* Get the candidates for commit */
  virtual Suggestion getCandidates() = 0;
  /* Is the last key was processed? */
  virtual bool handledKeyPress() = 0;
  /* Confirms that one of the sended candidates has been commited
   * @commited the string that was commited */
  virtual void candidateCommited(std::string commited) = 0;
};

class Layout {
  /* Layout Method handler */
  LayoutMth *mth;

  /* File input handler */
  QFile fin;
  /* Layout File */
  QJsonObject lf;
  /* Layout Descriptor */
  LayoutDesc lD;

  /* Load Layout Description. Used internaly */
  void loadDesc();
  /* Set typing method. Used internaly */
  void setMethod();

  QJsonObject sendLayout();

  /* Update with current settings */
  void updateWithSettings();
public:
  ~Layout();

  /* Load a layout from given @path */
  void loadLayout(QString path);

  /* Generates suggestions on the flow */
  Suggestion getSuggestion(int key, bool shift, bool ctrl, bool alt);
  /* Get the candidates for commit */
  Suggestion getCandidates();
  /* Handle special keys such as Enter, Backspace, Space etc keys */
  IMCommand handleSpecialKey(int key);
  /* Is the last key was processed? */
  bool handledKeyPress();
  /* Confirms that one of the sended candidates has been commited
   * @lcommited the string that was commited */
  void candidateCommited(std::string commited);
};

/* Global */
extern Layout *gLayout;

#endif /* end of include guard: LAYOUT_H */
