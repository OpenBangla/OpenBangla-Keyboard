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

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QEvent>

class QTimer;

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog {
Q_OBJECT

public:
  explicit AboutDialog(QWidget *parent = nullptr);

  ~AboutDialog() override;

protected:
  bool eventFilter(QObject *object, QEvent *event) override;

  void showEvent(QShowEvent *event) override;

private slots:

  void scroll();

  void on_btnClose_clicked();

  void on_btnLicense_toggled(bool checked);

  void on_labelDesc_linkActivated(const QString &link);

private:
  Ui::AboutDialog *ui;

  QTimer *timer;
};

#endif // ABOUTDIALOG_H
