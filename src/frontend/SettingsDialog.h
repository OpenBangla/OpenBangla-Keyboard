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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QMap>

/* Keycodes which we need to save 'Commit Raw Text' setting.
 * These must match with the codes from libengine. */
#define VC_A  0xA0B4
#define VC_B  0xA0B5
#define VC_C  0xA0B6
#define VC_D  0xA0B7
#define VC_E  0xA0B8
#define VC_F  0xA0B9
#define VC_G  0xA0BA
#define VC_H  0xA0BB
#define VC_I  0xA0BC
#define VC_J  0xA0BD
#define VC_K  0xA0BE
#define VC_L  0xA0BF
#define VC_M  0xA0C0
#define VC_N  0xA0C1
#define VC_O  0xA0C2
#define VC_P  0xA0C3
#define VC_Q  0xA0C4
#define VC_R  0xA0C5
#define VC_S  0xA0C6
#define VC_T  0xA0C7
#define VC_U  0xA0C8
#define VC_V  0xA0C9
#define VC_W  0xA0CA
#define VC_X  0xA0CB
#define VC_Y  0xA0CC
#define VC_Z  0xA0CD

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog {
Q_OBJECT

public:
  explicit SettingsDialog(QWidget *parent = nullptr);

  ~SettingsDialog() override;

  void updateSettings();

private slots:

  void on_buttonBox_accepted();

  void on_buttonBox_rejected();

  void on_btnClosePrevWin_toggled(bool checked);

  void on_btnShowPrevWin_toggled(bool checked);

  void on_btnCheckUpdate_toggled(bool checked);

private:
  Ui::SettingsDialog *ui;
  QMap<QString, int> rawTextKeys;
};

#endif // SETTINGSDIALOG_H
