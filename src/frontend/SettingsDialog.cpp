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

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    this->setFixedSize(QSize(this->width(), this->height()));
    ui->cmbOrientation->insertItems(0, {"Horizontal", "Vertical"});
    updateSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::updateSettings() {
    ui->btnClosePrevWin->setChecked(gSettings->getEnterKeyClosesPrevWin());
    ui->btnShowPrevWin->setChecked(gSettings->getShowCWPhonetic());
    ui->cmbOrientation->setCurrentIndex(gSettings->getCandidateWinHorizontal() ? 0 : 1);
}

void SettingsDialog::on_buttonBox_accepted()
{
    gSettings->setEnterKeyClosesPrevWin(ui->btnClosePrevWin->isChecked());
    gSettings->setShowCWPhonetic(ui->btnShowPrevWin->isChecked());
    gSettings->setCandidateWinHorizontal((ui->cmbOrientation->currentIndex() == 0));
}

void SettingsDialog::on_buttonBox_rejected()
{
    SettingsDialog::close();
}

void SettingsDialog::on_btnClosePrevWin_toggled(bool checked)
{
    ui->btnClosePrevWin->setText(checked ? "On" : "Off");
}

void SettingsDialog::on_btnShowPrevWin_toggled(bool checked)
{
    ui->btnShowPrevWin->setText(checked ? "On" : "Off");
}
