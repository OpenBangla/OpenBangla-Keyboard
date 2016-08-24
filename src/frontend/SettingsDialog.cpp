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

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "Settings.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    this->setFixedSize(QSize(this->width(), this->height()));
    updateSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::updateSettings() {
    ui->checkBoxPrevWin->setChecked(gSettings->getEnterKeyClosesPrevWin());
    ui->checkBoxCandidateWinHorizontal->setChecked(gSettings->getCandidateWinHorizontal());
}

void SettingsDialog::on_buttonBox_accepted()
{
    gSettings->setEnterKeyClosesPrevWin(ui->checkBoxPrevWin->isChecked());
    gSettings->setCandidateWinHorizontal(ui->checkBoxCandidateWinHorizontal->isChecked());
}

void SettingsDialog::on_buttonBox_rejected()
{
    SettingsDialog::close();
}
