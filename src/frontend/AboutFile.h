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

#ifndef ABOUTFILE_H
#define ABOUTFILE_H

#include <QDialog>
#include "Layout.h"

namespace Ui {
class AboutFile;
}

enum DialogType {
  AboutLayout
};

class AboutFile : public QDialog
{
    Q_OBJECT

public:
    explicit AboutFile(QWidget *parent = 0);
    ~AboutFile();

    void setDialogType(DialogType type);

private slots:
    void on_buttonOK_clicked();

private:
    Ui::AboutFile *ui;

    void loadLayoutInfo();
};

#endif // ABOUTFILE_H
