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
    ui->autoCorrect->setHeaderLabels({"Relplace", "With"});

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

void AutoCorrectDialog::on_autoCorrect_itemClicked(QTreeWidgetItem *item, int column)
{
    ui->txtReplace->setText(item->text(0));
    ui->txtWith->setText(item->text(1));
    ui->lblPreviewR->setText(item->text(0));
    ui->lblPreviewW->setText(item->text(1));
}

void AutoCorrectDialog::on_btnClear_clicked()
{
    ui->txtReplace->setText("");
    ui->txtWith->setText("");
}
