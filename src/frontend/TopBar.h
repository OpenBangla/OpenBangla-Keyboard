/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2016-2018 Muhammad Mominul Huque <mominul2082@gmail.com>
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

#ifndef TOPBAR_H
#define TOPBAR_H

#include <QMainWindow>

namespace Ui {
class TopBar;
}

class QActionGroup;

class QAction;

class QMenu;

class QSystemTrayIcon;

class LayoutViewer;

class AboutDialog;

class AboutFile;

class SettingsDialog;

class AutoCorrectDialog;

class QSimpleUpdater;

class TopBar : public QMainWindow {
Q_OBJECT

public:
  explicit TopBar(QWidget *parent = nullptr);

  ~TopBar() override;

protected:
  void closeEvent(QCloseEvent *event) override;

  bool eventFilter(QObject *object, QEvent *event) override;

private slots:

  void layoutMenuLayouts_clicked();

  void layoutMenuInstall_clicked();

  //void settingsMenuShowDialog_clicked();

  void aboutMenuLayout_clicked();

  void aboutMenuAbout_clicked();

  void quitMenuOnTray_clicked();

  void quitMenuQuit_clicked();

  void trayMenuRestore_clicked();

  void on_buttonAbout_clicked();

  void on_buttonSetLayout_clicked();

  void on_buttonShutdown_clicked();

  void on_buttonViewLayout_clicked();

  void on_buttonSettings_clicked();

private:
  Ui::TopBar *ui;
  bool canMoveTopbar;
  int pressedMouseX, pressedMouseY;
  QSystemTrayIcon *tray;
  QSimpleUpdater *updater;

  /* Dialogs */
  AboutDialog *aboutDialog;
  LayoutViewer *layoutViewer;
  SettingsDialog *settingsDialog;

  /* Layout Popup Menu */
  QMenu *layoutMenu;
  enum {
    MaxLayoutFiles = 10
  };
  QAction *layoutMenuLayouts[MaxLayoutFiles];
  QActionGroup *layoutMenuLayoutsGroup;
  QAction *layoutMenuInstall;
  /* Settings Popup Menu 
  QMenu *settingsMenu;
  QAction *settingsMenuShowDialog;*/
  /* About Popup Menu */
  QMenu *aboutMenu;
  QAction *aboutMenuLayout;
  QAction *aboutMenuAbout;
  QAction *aboutMenuUpdate;
  /* Quit Popup Menu */
  QMenu *quitMenu;
  QAction *quitMenuOnTray;
  QAction *quitMenuQuit;

  /* Tray Popup Menu */
  QMenu *trayMenu;
  QAction *trayMenuRestore;

  void SetupTopBar();

  void SetupPopupMenus();

  void SetupTrayIcon();

  void checkForUpdate();

  void RefreshLayouts();
};

#endif // TOPBAR_H
