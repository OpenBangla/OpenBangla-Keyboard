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
#include "AutoCorrectDialog.h"
#include "ui_AutoCorrectDialog.h"

AutoCorrectDialog::AutoCorrectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoCorrectDialog)
{
    ui->setupUi(this);

    ui->autoCorrect->setColumnCount(2);
    ui->autoCorrect->setHeaderLabels({"Replace", "With"});

    ui->btnClear->setEnabled(false);
    ui->btnUpdate->setEnabled(false);

    int items = loadEntries();
    ui->lblEntries->setText("Total entries: " + QString::number(items));
}

AutoCorrectDialog::~AutoCorrectDialog()
{
    delete ui;
}

int AutoCorrectDialog::loadEntries() {
    int items = 0;
    QJsonObject acList = dict.getEntries();
    QJsonObject::const_iterator iter = acList.constBegin();
    while(iter != acList.constEnd()) {
        addEntries(iter.key(), iter.value().toString());
        ++iter;
        ++items;
    }

    return items;
}

void AutoCorrectDialog::addEntries(QString replace, QString with) {
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->autoCorrect);
    item->setText(0, replace);
    item->setText(1, with);
}

void AutoCorrectDialog::on_buttonBox_accepted()
{
   //
}

void AutoCorrectDialog::on_buttonBox_rejected()
{
    AutoCorrectDialog::close();
}

void AutoCorrectDialog::on_btnUpdate_clicked()
{
    //
}

void AutoCorrectDialog::on_btnClear_clicked()
{
    ui->txtReplace->setText("");
    ui->txtWith->setText("");
}

void AutoCorrectDialog::on_txtReplace_textChanged(const QString &arg1)
{
    if(arg1 != "") {
        ui->lblPreviewR->setText(dict.convertBanglish(arg1));
        if(!ui->btnClear->isEnabled()) ui->btnClear->setEnabled(true);
        if(ui->txtWith->text() != "") {
            ui->btnUpdate->setEnabled(true);
        } else {
            ui->btnUpdate->setEnabled(false);
        }
    } else {
        ui->lblPreviewR->setText("");
        ui->btnClear->setEnabled(false);
        ui->btnUpdate->setEnabled(false);
    }
}

void AutoCorrectDialog::on_txtWith_textChanged(const QString &arg1)
{
    if(arg1 != "") {
        ui->lblPreviewW->setText(dict.convertBanglish(arg1));
        if(!ui->btnClear->isEnabled()) ui->btnClear->setEnabled(true);
        if(ui->txtReplace->text() != "") {
            ui->btnUpdate->setEnabled(true);
        } else {
            ui->btnUpdate->setEnabled(false);
        }
    } else {
        ui->lblPreviewW->setText("");
        ui->btnClear->setEnabled(false);
        ui->btnUpdate->setEnabled(false);
    }
}

void AutoCorrectDialog::on_autoCorrect_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    QString replace = current->text(0);
    QString with = current->text(1);

    ui->txtReplace->setText(replace);
    ui->lblPreviewR->setText(dict.convertBanglish(replace));

    ui->txtWith->setText(with);
    // smiley rule
    if(replace == with) {
        ui->lblPreviewW->setText(with);
    } else {
        ui->lblPreviewW->setText(dict.convertBanglish(with));
    }
}
