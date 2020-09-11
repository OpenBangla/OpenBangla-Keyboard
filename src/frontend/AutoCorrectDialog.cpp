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

#include <QTreeWidget>
#include <QJsonDocument>
#include "AutoCorrectDialog.h"
#include "ui_AutoCorrectDialog.h"

AutoCorrectDialog::AutoCorrectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoCorrectDialog) {
  ui->setupUi(this);

  ui->autoCorrect->setColumnCount(2);
  ui->autoCorrect->setHeaderLabels({"Replace", "With"});

  ui->btnClear->setEnabled(false);
  ui->btnUpdate->setEnabled(false);

  loadData();
  loadEntries();
}

AutoCorrectDialog::~AutoCorrectDialog() {
  delete ui;
}

void AutoCorrectDialog::loadData() {
  QFile dictFile(AutoCorrectFilePath());
  if (!dictFile.open(QIODevice::ReadOnly)) {
    LOG_ERROR("[AutoCorrect]: Error: Couldn't open autocorrect dictionary file!\n");
  }
  QByteArray data = dictFile.readAll();
  dictFile.close();

  QJsonDocument json(QJsonDocument::fromJson(data));
  dict = json.object();
  
  dictFile.setFileName(folders.getUserAutoCorrectFile());
  if (!dictFile.open(QIODevice::ReadOnly)) {
    LOG_ERROR("[AutoCorrect]: Error: Couldn't open user specific AutoCorrect file!\n");
  }
  data = dictFile.readAll();
  dictFile.close();

  QJsonDocument usrJson(QJsonDocument::fromJson(data));
  usrDict = usrJson.object();
}

QVariantMap AutoCorrectDialog::getEntries() {
  QVariantMap dct = dict.toVariantMap();

  /* Insert user's AutoCorrect entries.
   * If a conflict is found, we prefer user's entry.
   */
  QJsonObject::const_iterator iter = usrDict.constBegin();
  while (iter != usrDict.constEnd()) {
    dct.insert(iter.key(), iter.value());

    ++iter;
  }

  return dct;
}

void AutoCorrectDialog::loadEntries() {
  int items = 0;
  /* Block any signals for the treelist.
   * Otherwise it causes a Segmentation fault.
   */
  const QSignalBlocker blocker(ui->autoCorrect);
  // Clear the treelist
  ui->autoCorrect->clearSelection();
  ui->autoCorrect->clear();

  QVariantMap acList = getEntries();
  QVariantMap::const_iterator iter = acList.constBegin();
  while (iter != acList.constEnd()) {
    addEntryInViewer(iter.key(), iter.value().toString());
    ++iter;
    ++items;
  }

  // Sort list
  ui->autoCorrect->sortItems(0, Qt::SortOrder::AscendingOrder);
  // Update
  ui->lblEntries->setText("Total entries: " + QString::number(items));
}

void AutoCorrectDialog::addEntryInViewer(QString replace, QString with) {
  QTreeWidgetItem *item = new QTreeWidgetItem(ui->autoCorrect);
  item->setText(0, replace);
  item->setText(1, with);
}

void AutoCorrectDialog::on_buttonBox_accepted() {
  // Save changes in user specific auto correct file.
  QFile saveFile(folders.getUserAutoCorrectFile());
  if (!saveFile.open(QIODevice::WriteOnly)) {
    LOG_ERROR("[AutoCorrect:Save]: Error couldn't open save file.\n");
    return;
  }

  QJsonDocument json(usrDict);
  saveFile.write(json.toJson());

  saveFile.close();
}

void AutoCorrectDialog::on_buttonBox_rejected() {
  AutoCorrectDialog::close();
}

void AutoCorrectDialog::on_btnUpdate_clicked() {
  usrDict.insert(ui->txtReplace->text().trimmed(), ui->txtWith->text().trimmed());
  loadEntries();
}

void AutoCorrectDialog::on_btnClear_clicked() {
  ui->txtReplace->setText("");
  ui->txtWith->setText("");
}

void AutoCorrectDialog::on_txtReplace_textChanged(const QString &arg1) {
  if (arg1 != "") {
    ui->lblPreviewR->setText(phonetic.convert(arg1));
    if (!ui->btnClear->isEnabled()) {
      ui->btnClear->setEnabled(true);
    }

    ui->btnUpdate->setEnabled(ui->txtWith->text() != "");

  } else {
    ui->lblPreviewR->setText("");
    ui->btnClear->setEnabled(false);
    ui->btnUpdate->setEnabled(false);
  }
}

void AutoCorrectDialog::on_txtWith_textChanged(const QString &arg1) {
  if (arg1 != "") {
    // smiley rule
    if (ui->txtReplace->text() == arg1) {
      ui->lblPreviewW->setText(arg1);
    } else {
      ui->lblPreviewW->setText(phonetic.convert(arg1));
    }

    if (!ui->btnClear->isEnabled())
      ui->btnClear->setEnabled(true);
    ui->btnUpdate->setEnabled(ui->txtReplace->text() != "");
  } else {
    ui->lblPreviewW->setText("");
    ui->btnClear->setEnabled(false);
    ui->btnUpdate->setEnabled(false);
  }
}

void AutoCorrectDialog::on_autoCorrect_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
  ui->txtReplace->setText(current->text(0));
  ui->txtWith->setText(current->text(1));
}
