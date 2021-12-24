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
  explicit TopBar(bool darkIcon = false, QWidget *parent = nullptr);

  ~TopBar() override;

protected:
  void closeEvent(QCloseEvent *event) override;

  bool eventFilter(QObject *object, QEvent *event) override;

private slots:

  void layoutMenuLayouts_clicked();

  void layoutMenuInstall_clicked();

  void iconMenuLayout_clicked();

  void iconMenuAbout_clicked();

  void on_buttonIcon_clicked();

  void on_buttonSetLayout_clicked();

  void on_buttonShutdown_clicked();

  void on_buttonViewLayout_clicked();

  void on_buttonSettings_clicked();

private:
  Ui::TopBar *ui;
  bool canMoveTopbar = false;
  QString m_iconTheme;
  bool positionChanged = false;
  int pressedMouseX, pressedMouseY;
  QSystemTrayIcon *tray;

#ifndef NO_UPDATE_CHECK
  QSimpleUpdater *updater;
#endif

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
  /* Icon Button Popup Menu */
  QMenu *iconMenu;
  QAction *iconMenuHide;
  QAction *iconMenuLayout;
  QAction *iconMenuAbout;
  QAction *iconMenuUpdate;

  /* Tray Popup Menu */
  QMenu *trayMenu;
  QMenu *trayOutputMode;
  QAction *trayOutputModeUnicode;
  QAction *trayOutputModeANSI;
  QActionGroup *trayOutputModeGroup;
  QAction *trayLayoutViewer;
  QAction *traySettings;
  QAction *trayTopBarVisibility;
  QAction *trayQuit;

  void SetupTopBar();

  void SetupPopupMenus();

  void SetupTrayIcon();

  void checkForUpdate();

  void RefreshLayouts();

  void DataMigration();
};

#endif // TOPBAR_H
