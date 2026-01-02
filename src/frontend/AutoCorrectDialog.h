/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2018 Muhammad Mominul Huque <mominul2082@gmail.com>
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

#ifndef AUTOCORRECTDIALOG_H
#define AUTOCORRECTDIALOG_H

#include <QDialog>
#include "AvroPhonetic.h"

class QTreeWidgetItem;

namespace Ui {
class AutoCorrectDialog;
}

class AutoCorrectDialog : public QDialog {
Q_OBJECT

public:
  explicit AutoCorrectDialog(QWidget *parent = nullptr);

  ~AutoCorrectDialog() override;

private slots:

  void on_buttonBox_accepted();

  void on_buttonBox_rejected();

  void on_btnUpdate_clicked();

  void on_btnClear_clicked();

  void on_txtReplace_textChanged(const QString &arg1);

  void on_txtWith_textChanged(const QString &arg1);

  void on_autoCorrect_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
  Ui::AutoCorrectDialog *ui;
  AvroPhonetic phonetic;

  QJsonObject dict;
  QJsonObject usrDict;

  void loadData();
  /* Get AutoCorrect entries */
  QVariantMap getEntries();
  void loadEntries();

  void addEntryInViewer(QString replace, QString with);
};

#endif // AUTOCORRECTDIALOG_H
