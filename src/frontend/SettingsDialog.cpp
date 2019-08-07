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

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "Settings.h"
#include "AutoCorrectDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog) {
  ui->setupUi(this);
  autoCorrectDialog = new AutoCorrectDialog(this);

  this->setFixedSize(QSize(this->width(), this->height()));
  ui->cmbOrientation->insertItems(0, {"Horizontal", "Vertical"});
  ui->cmbRawTxt->addItem("None");
  rawTextKeys = {
    {"Ctrl + Q", VC_Q},
    {"Ctrl + W", VC_W},
    {"Ctrl + E", VC_E},
    {"Ctrl + R", VC_R},
    {"Ctrl + T", VC_T},
    {"Ctrl + Y", VC_Y},
    {"Ctrl + U", VC_U},
    {"Ctrl + I", VC_I},
    {"Ctrl + O", VC_O},
    {"Ctrl + P", VC_P},
    {"Ctrl + A", VC_A},
    {"Ctrl + S", VC_S},
    {"Ctrl + D", VC_D},
    {"Ctrl + F", VC_F},
    {"Ctrl + G", VC_G},
    {"Ctrl + H", VC_H},
    {"Ctrl + J", VC_J},
    {"Ctrl + K", VC_K},
    {"Ctrl + L", VC_L},
    {"Ctrl + Z", VC_Z},
    {"Ctrl + X", VC_X},
    {"Ctrl + C", VC_C},
    {"Ctrl + V", VC_V},
    {"Ctrl + B", VC_B},
    {"Ctrl + V", VC_V},
    {"Ctrl + N", VC_N},
    {"Ctrl + M", VC_M},
  };
  ui->cmbRawTxt->addItems(rawTextKeys.keys());
  updateSettings();
}

SettingsDialog::~SettingsDialog() {
  delete autoCorrectDialog;
  delete ui;
}

void SettingsDialog::updateSettings() {
  // Phonetic Keyboard Layout
  ui->btnClosePrevWin->setChecked(gSettings->getEnterKeyClosesPrevWin());
  ui->btnShowPrevWin->setChecked(gSettings->getShowCWPhonetic());
  ui->cmbOrientation->setCurrentIndex(gSettings->getCandidateWinHorizontal() ? 0 : 1);
  int rawTextKey = gSettings->getCommitRaw();
  if(rawTextKey == 0) {
    ui->cmbRawTxt->setCurrentText("None");
  } else {
    ui->cmbRawTxt->setCurrentText(rawTextKeys.keys(rawTextKey).first());
  }

  // Fixed Keyboard Layout
  ui->btnAutoVowel->setChecked(gSettings->getAutoVowelFormFixed());
  ui->btnAutoChandra->setChecked(gSettings->getAutoChandraPosFixed());
  ui->btnOldReph->setChecked(gSettings->getOldReph());
  ui->btnKarJoining->setChecked(gSettings->getTraditionalKarFixed());
  ui->btnNumberpad->setChecked(gSettings->getNumberPadFixed());

  ui->btnCheckUpdate->setChecked(gSettings->getUpdateCheck());
}

void SettingsDialog::on_buttonBox_accepted() {
  // Phonetic Keyboard Layout
  gSettings->setEnterKeyClosesPrevWin(ui->btnClosePrevWin->isChecked());
  gSettings->setShowCWPhonetic(ui->btnShowPrevWin->isChecked());
  gSettings->setCandidateWinHorizontal((ui->cmbOrientation->currentIndex() == 0));
  QString rawTextKey = ui->cmbRawTxt->currentText();
  if(rawTextKey == "None") {
    gSettings->setCommitRaw(0);
  } else {
    gSettings->setCommitRaw(rawTextKeys.value(rawTextKey));
  }

  // Fixed Keyboard Layout
  gSettings->setAutoVowelFormFixed(ui->btnAutoVowel->isChecked());
  gSettings->setAutoChandraPosFixed(ui->btnAutoChandra->isChecked());
  gSettings->setOldReph(ui->btnOldReph->isChecked());
  gSettings->setTraditionalKarFixed(ui->btnKarJoining->isChecked());
  gSettings->setNumberPadFixed(ui->btnNumberpad->isChecked());

  gSettings->setUpdateCheck(ui->btnCheckUpdate->isChecked());
}

void SettingsDialog::on_buttonBox_rejected() {
  SettingsDialog::close();
}

void SettingsDialog::on_btnClosePrevWin_toggled(bool checked) {
  ui->btnClosePrevWin->setText(checked ? "On" : "Off");
}

void SettingsDialog::on_btnShowPrevWin_toggled(bool checked) {
  ui->btnShowPrevWin->setText(checked ? "On" : "Off");
}

void SettingsDialog::on_btnCheckUpdate_toggled(bool checked) {
  ui->btnCheckUpdate->setText(checked ? "On" : "Off");
}

void SettingsDialog::on_btnAutoVowel_toggled(bool checked)
{
  ui->btnAutoVowel->setText(checked ? "On" : "Off");
}

void SettingsDialog::on_btnKarJoining_toggled(bool checked)
{
  ui->btnKarJoining->setText(checked ? "On" : "Off");
}

void SettingsDialog::on_btnAutoChandra_toggled(bool checked)
{
  ui->btnAutoChandra->setText(checked ? "On" : "Off");
}

void SettingsDialog::on_btnOldReph_toggled(bool checked)
{
  ui->btnOldReph->setText(checked ? "On" : "Off");
}

void SettingsDialog::on_btnNumberpad_toggled(bool checked)
{
  ui->btnNumberpad->setText(checked ? "On" : "Off");
}

void SettingsDialog::on_btnACUpdate_clicked()
{
  autoCorrectDialog->show();
}
