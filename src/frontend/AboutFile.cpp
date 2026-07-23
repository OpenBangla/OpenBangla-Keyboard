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

#include <QFile>
#include <QTextOption>
#include "AboutFile.h"
#include "ui_AboutFile.h"

AboutFile::AboutFile(QString iconTheme, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutFile) {
  ui->setupUi(this);

  bool darkMode = (iconTheme == "white");
  QFile qss(darkMode ? ":/styles/dark.qss" : ":/styles/light.qss");
  if (qss.open(QFile::ReadOnly | QFile::Text)) {
    setStyleSheet(QString::fromUtf8(qss.readAll()));
  }

  // Justify the developer comment so multi-line text reads as a clean block.
  QTextOption commentOption = ui->txtDevComment->document()->defaultTextOption();
  commentOption.setAlignment(Qt::AlignJustify);
  ui->txtDevComment->document()->setDefaultTextOption(commentOption);

  this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
}

AboutFile::~AboutFile() {
  delete ui;
}

void AboutFile::setDialogType(DialogType type) {
  if (type == AboutLayout) {
    this->setWindowTitle("About This Layout");
    loadLayoutInfo();
  }
}

void AboutFile::loadLayoutInfo() {
  LayoutDesc desc = gLayout->getDesc();
  ui->txtName->setText(desc.name);
  ui->txtVar->setText(desc.ver);
  ui->txtDevName->setText(desc.devName);
  ui->txtDevComment->setPlainText(desc.devComment);
}

void AboutFile::on_buttonOK_clicked() {
  AboutFile::close();
}
