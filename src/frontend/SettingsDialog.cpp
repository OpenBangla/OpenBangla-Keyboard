/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2016-2022 Muhammad Mominul Huque <mominul2082@gmail.com>
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
  ui->cmbEncoding->insertItems(0, {"Unicode", "ANSI"});
  ui->cmbKarOrder->insertItems(0, {"Modern", "Old"});

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
  // General Group
  connect(ui->btnEnterClosePW, &QPushButton::toggled, [=](bool checked) {
    ui->btnEnterClosePW->setText(checked ? "On" : "Off");
  });
  connect(ui->btnIncludeEnglishPrevWin, &QPushButton::toggled, [=](bool checked) {
    ui->btnIncludeEnglishPrevWin->setText(checked ? "On" : "Off");
  });
  connect(ui->btnSmartQuote, &QPushButton::toggled, [=](bool checked) {
    ui->btnSmartQuote->setText(checked ? "On" : "Off");
  });
  
  // Phonetic Keyboard Layout Group.
  connect(ui->btnSuggestionPhonetic, &QPushButton::toggled, [=](bool checked) {
    ui->btnSuggestionPhonetic->setText(checked ? "On" : "Off");
    // Control other Preview window related settings.
    ui->btnACUpdate->setEnabled(checked);
  });
  connect(ui->btnACUpdate, &QPushButton::clicked, [=]() {
    autoCorrectDialog->open();
  });

  // Fixed Keyboard Layout Group.
  connect(ui->btnSuggestionFixed, &QPushButton::toggled, [=](bool checked) {
    ui->btnSuggestionFixed->setText(checked ? "On" : "Off");
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
    // Emit signal
    emit this->finished(1);
  });
  connect(ui->btnCancel, &QPushButton::clicked, [=]() {
    this->hide();
  });
}

void SettingsDialog::updateSettings() {
  // General Group
  ui->btnEnterClosePW->setChecked(gSettings->getEnterKeyClosesPrevWin());
  ui->btnSmartQuote->setChecked(gSettings->getSmartQuoting());
  ui->cmbOrientation->setCurrentIndex(gSettings->getCandidateWinHorizontal() ? 0 : 1);
  ui->btnIncludeEnglishPrevWin->setChecked(gSettings->getSuggestionIncludeEnglish());
  ui->cmbEncoding->setCurrentIndex(gSettings->getANSIEncoding() ? 1 : 0);

  // Phonetic Keyboard Layout Group.
  ui->btnSuggestionPhonetic->setChecked(gSettings->getShowCWPhonetic());

  // Fixed Keyboard Layout Group.
  ui->btnSuggestionFixed->setChecked(gSettings->getShowPrevWinFixed());
  ui->btnAutoVowel->setChecked(gSettings->getAutoVowelFormFixed());
  ui->btnAutoChandra->setChecked(gSettings->getAutoChandraPosFixed());
  ui->cmbKarOrder->setCurrentIndex(gSettings->getFixedOldKarOrder() ? 1 : 0);
  ui->btnOldReph->setChecked(gSettings->getOldReph());
  ui->btnKarJoining->setChecked(gSettings->getTraditionalKarFixed());
  ui->btnNumberpad->setChecked(gSettings->getNumberPadFixed());

  ui->btnCheckUpdate->setChecked(gSettings->getUpdateCheck());
}

void SettingsDialog::saveSettings() {
  // General Group
  gSettings->setEnterKeyClosesPrevWin(ui->btnEnterClosePW->isChecked());
  gSettings->setSmartQuoting(ui->btnSmartQuote->isChecked());
  gSettings->setCandidateWinHorizontal((ui->cmbOrientation->currentIndex() == 0));
  gSettings->setSuggestionIncludeEnglish(ui->btnIncludeEnglishPrevWin->isChecked());
  gSettings->setANSIEncoding(ui->cmbEncoding->currentIndex() == 1);

  // Phonetic Keyboard Layout Group.
  gSettings->setShowCWPhonetic(ui->btnSuggestionPhonetic->isChecked());

  // Fixed Keyboard Layout Group.
  gSettings->setShowPrevWinFixed(ui->btnSuggestionFixed->isChecked());
  gSettings->setAutoVowelFormFixed(ui->btnAutoVowel->isChecked());
  gSettings->setAutoChandraPosFixed(ui->btnAutoChandra->isChecked());
  gSettings->setFixedOldKarOrder(ui->cmbKarOrder->currentIndex() == 1);
  gSettings->setOldReph(ui->btnOldReph->isChecked());
  gSettings->setTraditionalKarFixed(ui->btnKarJoining->isChecked());
  gSettings->setNumberPadFixed(ui->btnNumberpad->isChecked());

  gSettings->setUpdateCheck(ui->btnCheckUpdate->isChecked());
}

void SettingsDialog::closeEvent(QCloseEvent *event) {
  this->hide();
  event->ignore();
}
