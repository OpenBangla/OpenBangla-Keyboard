/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2016-2018 Muhammad Mominul Huque <mominul2082@gmail.com>
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

#include <QCloseEvent>
#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "Settings.h"
#include "AutoCorrectDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog) {
  ui->setupUi(this);
  autoCorrectDialog = new AutoCorrectDialog(this);

  ui->cmbOrientation->insertItems(0, {"Horizontal", "Vertical"});

#ifdef NO_UPDATE_CHECK
  ui->lblUpdateCheck->hide();
  ui->btnCheckUpdate->hide();
#endif

  implementSignals();
  updateSettings();
}

SettingsDialog::~SettingsDialog() {
  delete autoCorrectDialog;
  delete ui;
}

void SettingsDialog::implementSignals() {
  // Phonetic Keyboard Layout Group.
  connect(ui->btnSuggestionPhonetic, &QPushButton::toggled, [=](bool checked) {
    ui->btnSuggestionPhonetic->setText(checked ? "On" : "Off");
    // Control other Preview window related settings.
    ui->btnIncludeEnglishPrevWin->setEnabled(checked);
    ui->btnACUpdate->setEnabled(checked);
  });
  connect(ui->btnEnterClosePW, &QPushButton::toggled, [=](bool checked) {
    ui->btnEnterClosePW->setText(checked ? "On" : "Off");
  });
  connect(ui->btnIncludeEnglishPrevWin, &QPushButton::toggled, [=](bool checked) {
    ui->btnIncludeEnglishPrevWin->setText(checked ? "On" : "Off");
  });
  connect(ui->btnACUpdate, &QPushButton::clicked, [=]() {
    autoCorrectDialog->open();
  });

  // Fixed Keyboard Layout Group.
  connect(ui->btnSuggestionFixed, &QPushButton::toggled, [=](bool checked) {
    ui->btnSuggestionFixed->setText(checked ? "On" : "Off");
    ui->btnIncludeEnglishFixed->setEnabled(checked);
  });
  connect(ui->btnIncludeEnglishFixed, &QPushButton::toggled, [=](bool checked) {
    ui->btnIncludeEnglishFixed->setText(checked ? "On" : "Off");
  });
  connect(ui->btnAutoVowel, &QPushButton::toggled, [=](bool checked) {
    ui->btnAutoVowel->setText(checked ? "On" : "Off");
  });
  connect(ui->btnKarJoining, &QPushButton::toggled, [=](bool checked) {
    ui->btnKarJoining->setText(checked ? "On" : "Off");
  });
  connect(ui->btnAutoChandra, &QPushButton::toggled, [=](bool checked) {
    ui->btnAutoChandra->setText(checked ? "On" : "Off");
  });
  connect(ui->btnOldReph, &QPushButton::toggled, [=](bool checked) {
    ui->btnOldReph->setText(checked ? "On" : "Off");
  });
  connect(ui->btnNumberpad, &QPushButton::toggled, [=](bool checked) {
    ui->btnNumberpad->setText(checked ? "On" : "Off");
  });

  // General 
  connect(ui->btnCheckUpdate, &QPushButton::toggled, [=](bool checked) {
    ui->btnCheckUpdate->setText(checked ? "On" : "Off");
  });

  connect(ui->btnOK, &QPushButton::clicked, [=]() {
    saveSettings();
    this->hide();
  });
  connect(ui->btnCancel, &QPushButton::clicked, [=]() {
    this->hide();
  });
}

void SettingsDialog::updateSettings() {
  // Phonetic Keyboard Layout Group.
  ui->btnEnterClosePW->setChecked(gSettings->getEnterKeyClosesPrevWin());
  ui->btnSuggestionPhonetic->setChecked(gSettings->getShowCWPhonetic());
  ui->cmbOrientation->setCurrentIndex(gSettings->getCandidateWinHorizontal() ? 0 : 1);
  ui->btnIncludeEnglishPrevWin->setChecked(gSettings->getIncludeEnglishPhonetic());

  // Fixed Keyboard Layout Group.
  ui->btnSuggestionFixed->setChecked(gSettings->getShowPrevWinFixed());
  ui->btnIncludeEnglishFixed->setChecked(gSettings->getIncludeEnglishFixed());
  ui->btnAutoVowel->setChecked(gSettings->getAutoVowelFormFixed());
  ui->btnAutoChandra->setChecked(gSettings->getAutoChandraPosFixed());
  ui->btnOldReph->setChecked(gSettings->getOldReph());
  ui->btnKarJoining->setChecked(gSettings->getTraditionalKarFixed());
  ui->btnNumberpad->setChecked(gSettings->getNumberPadFixed());

  ui->btnCheckUpdate->setChecked(gSettings->getUpdateCheck());
}

void SettingsDialog::saveSettings() {
  // Phonetic Keyboard Layout Group.
  gSettings->setEnterKeyClosesPrevWin(ui->btnEnterClosePW->isChecked());
  gSettings->setShowCWPhonetic(ui->btnSuggestionPhonetic->isChecked());
  gSettings->setCandidateWinHorizontal((ui->cmbOrientation->currentIndex() == 0));
  gSettings->setIncludeEnglishPhonetic(ui->btnIncludeEnglishPrevWin->isChecked());

  // Fixed Keyboard Layout Group.
  gSettings->setShowPrevWinFixed(ui->btnSuggestionFixed->isChecked());
  gSettings->setIncludeEnglishFixed(ui->btnIncludeEnglishFixed->isChecked());
  gSettings->setAutoVowelFormFixed(ui->btnAutoVowel->isChecked());
  gSettings->setAutoChandraPosFixed(ui->btnAutoChandra->isChecked());
  gSettings->setOldReph(ui->btnOldReph->isChecked());
  gSettings->setTraditionalKarFixed(ui->btnKarJoining->isChecked());
  gSettings->setNumberPadFixed(ui->btnNumberpad->isChecked());

  gSettings->setUpdateCheck(ui->btnCheckUpdate->isChecked());
}

void SettingsDialog::closeEvent(QCloseEvent *event) {
  this->hide();
  event->ignore();
}
