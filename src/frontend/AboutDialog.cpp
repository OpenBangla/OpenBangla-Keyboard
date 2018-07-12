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

#include <QTimer>
#include <QDesktopServices>
#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog) {
  ui->setupUi(this);

  this->setFixedSize(this->width(), this->height());

  // Set version
  ui->labelVer->setText(PROJECT_VERSION);

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(scroll()));
  timer->setInterval(30);
  timer->start();

  ui->txtLicense->setVisible(false);
  ui->labelDesc->installEventFilter(this);
}

AboutDialog::~AboutDialog() {
  delete ui;
  delete timer;
}

void AboutDialog::scroll() {
  QPoint point;
  point.setX(ui->labelDesc->x());
  if (ui->labelDesc->y() != -780) {
    point.setY(ui->labelDesc->y() - 1);
    ui->labelDesc->move(point);
  } else {
    point.setY(300);
    ui->labelDesc->move(point);
  }
}

void AboutDialog::showEvent(QShowEvent *event) {
  QDialog::showEvent(event);
  ui->labelDesc->move(ui->labelDesc->x(), 300);
  return;
}

void AboutDialog::on_btnClose_clicked() {
  this->close();
}

void AboutDialog::on_btnLicense_toggled(bool checked) {
  ui->txtLicense->setVisible(checked);
  ui->labelDesc->setVisible(!checked);
  if (checked) {
    timer->stop();
  } else {
    timer->start();
  }
}

bool AboutDialog::eventFilter(QObject *object, QEvent *event) {
  if (object == ui->labelDesc) {
    switch (event->type()) {
    case QEvent::Enter:timer->stop();
      break;
    case QEvent::Leave:timer->start();
      break;
    default:break;
    }
  }
  return QObject::eventFilter(object, event);
}

void AboutDialog::on_labelDesc_linkActivated(const QString &link) {
  QDesktopServices::openUrl(QUrl(link));
}
