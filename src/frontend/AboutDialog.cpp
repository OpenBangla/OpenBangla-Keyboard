/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2020 Muhammad Mominul Huque <mominul2082@gmail.com>
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

#include <QDesktopServices>
#include <QUrl>
#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog) {
  ui->setupUi(this);

  this->setFixedSize(this->width(), this->height());

  // Set version
  ui->labelVer->setText(PROJECT_VERSION);
}

AboutDialog::~AboutDialog() {
  delete ui;
}

void AboutDialog::on_lblWebsite_linkActivated(const QString &link) {
  QDesktopServices::openUrl(QUrl(link));
}

void AboutDialog::on_lblContrib_linkActivated(const QString &link) {
  QDesktopServices::openUrl(QUrl(link));
}

void AboutDialog::on_lblLicense_linkActivated(const QString &link) {
  QDesktopServices::openUrl(QUrl(link));
}
