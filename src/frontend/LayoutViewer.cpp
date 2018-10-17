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

#include "LayoutViewer.h"
#include "Settings.h"
#include "AboutFile.h"
#include "ui_LayoutViewer.h"

LayoutViewer::LayoutViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LayoutViewer) {
  ui->setupUi(this);
  aboutDialog = new AboutFile(this);
  ui->labelImage->setAlignment(Qt::AlignCenter);
  this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
  this->move(gSettings->getLayoutViewerWindowPosition());
}

LayoutViewer::~LayoutViewer() {
  delete aboutDialog;
  delete ui;
}

void LayoutViewer::refreshLayoutViewer() {
  desc = gLayout->getDesc();
  this->setWindowTitle(desc.name + " :: Layout Viewer");
  on_viewNormal_clicked();
  // This refreshes Layout Info Dialog
  aboutDialog->setDialogType(AboutLayout);
}

void LayoutViewer::showLayoutInfoDialog() {
  aboutDialog->setDialogType(AboutLayout);
  aboutDialog->show();
}

void LayoutViewer::on_buttonClose_clicked() {
  gSettings->setLayoutViewerWindowPosition(this->pos());
  LayoutViewer::close();
}

void LayoutViewer::on_buttonAboutLayout_clicked() {
  showLayoutInfoDialog();
}

void LayoutViewer::on_viewNormal_clicked() {
  ui->labelImage->setText("");
  image.loadFromData(QByteArray::fromBase64(desc.image0));
  ui->labelImage->setPixmap(QPixmap::fromImage(image));
  ui->labelImage->adjustSize();


  
//  this->setFixedHeight(ui->labelImage->height() + ui->labelImage->y());
//  this->setFixedWidth(ui->labelImage->width());


//  ui->labelImage->setScaledContents(true);
//  ui->labelImage->setFixedSize(780,360);


  ui->viewNormal->setChecked(true);
}

void LayoutViewer::on_viewAltGr_clicked() {
  if (desc.image1.size() != 0) {
    image.loadFromData(QByteArray::fromBase64(desc.image1));
    ui->labelImage->setPixmap(QPixmap::fromImage(image));
    ui->labelImage->adjustSize();


//    ui->labelImage->setScaledContents(true);
//    ui->labelImage->setFixedSize(780,361);


    //    this->setFixedHeight(ui->labelImage->height() + ui->labelImage->y());
//    this->setFixedWidth(ui->labelImage->width());
    ui->viewAltGr->setChecked(true);
  } else {
    ui->labelImage->setText("No image to display!");
  }
}
