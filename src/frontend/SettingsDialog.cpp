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
#include <QButtonGroup>
#include <QFile>
#include <QIcon>
#include <QLabel>
#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "Settings.h"
#include "AutoCorrectDialog.h"
#include "ToggleSwitch.h"

SettingsDialog::SettingsDialog(bool darkMode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    m_darkMode(darkMode) {
  ui->setupUi(this);
  autoCorrectDialog = new AutoCorrectDialog(this);

  applyTheme();

  ui->cmbOrientation->insertItems(0, {"Horizontal", "Vertical"});
  ui->cmbEncoding->insertItems(0, {"Unicode", "ANSI"});
  ui->cmbKarOrder->insertItems(0, {"Modern", "Old"});

  setupSidebar();
  implementSignals();
  updateSettings();
}

void SettingsDialog::applyTheme() {
  // Load the matching theme stylesheet. Applied on the dialog so it cascades
  // to every child widget, including the ToggleSwitch qproperty colors.
  QString qssPath = m_darkMode ? ":/styles/dark.qss" : ":/styles/light.qss";
  QFile file(qssPath);
  if (file.open(QFile::ReadOnly | QFile::Text)) {
    setStyleSheet(QString::fromUtf8(file.readAll()));
  }

  // Icons ship in black/white variants; pick the one that reads on the theme.
  QString theme = m_darkMode ? "white" : "black";
  auto icon = [&](const QString &name) {
    return QIcon(":/images/" + theme + "/" + name + ".svg");
  };
  ui->sidebarGeneral->setIcon(icon("settings"));
  ui->sidebarPhonetic->setIcon(icon("translate"));
  ui->sidebarFixed->setIcon(icon("layout"));
  ui->pageGeneralIcon->setPixmap(icon("settings").pixmap(24, 24));
  ui->pagePhoneticIcon->setPixmap(icon("translate").pixmap(24, 24));
  ui->pageFixedIcon->setPixmap(icon("layout").pixmap(24, 24));
}

void SettingsDialog::setupSidebar() {
  auto *group = new QButtonGroup(this);
  group->setExclusive(true);
  group->addButton(ui->sidebarGeneral, 0);
  group->addButton(ui->sidebarPhonetic, 1);
  group->addButton(ui->sidebarFixed, 2);
  ui->sidebarGeneral->setChecked(true);
  ui->pages->setCurrentIndex(0);
  connect(group, QOverload<int>::of(&QButtonGroup::idClicked),
          ui->pages, &QStackedWidget::setCurrentIndex);
}

void SettingsDialog::bindToggle(ToggleSwitch *sw, QLabel *status) {
  auto update = [status](bool on) { status->setText(on ? "On" : "Off"); };
  connect(sw, &QAbstractButton::toggled, status, update);
  update(sw->isChecked());
}

SettingsDialog::~SettingsDialog() {
  delete autoCorrectDialog;
  delete ui;
}

void SettingsDialog::implementSignals() {
  // General Group
  bindToggle(ui->btnEnterClosePW, ui->btnEnterClosePWStatus);
  bindToggle(ui->btnIncludeEnglishPrevWin, ui->btnIncludeEnglishPrevWinStatus);
  bindToggle(ui->btnSmartQuote, ui->btnSmartQuoteStatus);

  // Phonetic Keyboard Layout Group.
  bindToggle(ui->btnSuggestionPhonetic, ui->btnSuggestionPhoneticStatus);
  // Control other Preview window related settings.
  connect(ui->btnSuggestionPhonetic, &QAbstractButton::toggled,
          ui->btnACUpdate, &QWidget::setEnabled);
  ui->btnACUpdate->setEnabled(ui->btnSuggestionPhonetic->isChecked());
  connect(ui->btnACUpdate, &QPushButton::clicked, [=]() {
    autoCorrectDialog->open();
  });

  // Fixed Keyboard Layout Group.
  bindToggle(ui->btnSuggestionFixed, ui->btnSuggestionFixedStatus);
  bindToggle(ui->btnAutoVowel, ui->btnAutoVowelStatus);
  bindToggle(ui->btnKarJoining, ui->btnKarJoiningStatus);
  bindToggle(ui->btnAutoChandra, ui->btnAutoChandraStatus);
  bindToggle(ui->btnOldReph, ui->btnOldRephStatus);
  bindToggle(ui->btnNumberpad, ui->btnNumberpadStatus);

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
}

void SettingsDialog::closeEvent(QCloseEvent *event) {
  this->hide();
  event->ignore();
}
