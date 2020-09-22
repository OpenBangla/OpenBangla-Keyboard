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

#include <QSystemTrayIcon>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMenu>
#include <QDir>
#include "TopBar.h"
#include "Layout.h"
#include "Settings.h"
#include "FileSystem.h"
#include "LayoutViewer.h"
#include "AboutDialog.h"
#include "SettingsDialog.h"
#include "LayoutConverter.h"
#include "AutoCorrectDialog.h"
#include "QSimpleUpdater.h"
#include "ui_TopBar.h"

static const QString DEFS_URL = "https://raw.githubusercontent.com/OpenBangla/OpenBangla-Keyboard/master/UPDATES.json";

TopBar::TopBar(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TopBar) {
  ui->setupUi(this);

  gLayout = new Layout();
  gSettings = new Settings();
  updater = QSimpleUpdater::getInstance();

  /* Dialogs */
  aboutDialog = new AboutDialog(Q_NULLPTR);
  layoutViewer = new LayoutViewer(Q_NULLPTR);
  settingsDialog = new SettingsDialog(Q_NULLPTR);

  ui->buttonIcon->installEventFilter(this);

  SetupTopBar();
  SetupPopupMenus();
  SetupTrayIcon();
  DataMigration();

  if (gSettings->getUpdateCheck()) {
    checkForUpdate();
  }
}

TopBar::~TopBar() {
  /* Dialogs */
  delete layoutViewer;
  delete settingsDialog;
  delete aboutDialog;

  delete gLayout;
  delete gSettings;

  delete ui;
}

void TopBar::SetupTopBar() {
#if defined(linux) || defined(__linux__) || defined(__linux)
  this->setWindowFlags(
      Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint |
          Qt::WindowDoesNotAcceptFocus | Qt::NoDropShadowWindowHint);
#else
  this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
#endif
  this->setFixedSize(QSize(this->width(), this->height()));

  if (gSettings->getTopBarWindowPosition() == QPoint(0, 0)) {
    int width = this->frameGeometry().width();
    int height = this->frameGeometry().height();
    QDesktopWidget wid;

    int screenWidth = wid.screen()->width();
    int screenHeight = wid.screen()->height();

    this->setGeometry((screenWidth / 2) - (width / 2), (screenHeight / 2) - (height / 2), width, height);
  } else {
    move(gSettings->getTopBarWindowPosition());
  }
}

void TopBar::checkForUpdate() {
  updater->setModuleVersion(DEFS_URL, PROJECT_VERSION);
  updater->setNotifyOnUpdate(DEFS_URL, true);
  updater->setDownloaderEnabled(DEFS_URL, false);
  updater->checkForUpdates(DEFS_URL);
}

void TopBar::SetupPopupMenus() {
  // Layout Popup Menu
  layoutMenu = new QMenu("Select keyboard layout", this);
  layoutMenu->setIcon(QIcon(":/images/keyboard_layout.png"));
  connect(layoutMenu, &QMenu::aboutToHide, [=]() {
    ui->buttonSetLayout->setChecked(false);
  });

  layoutMenuInstall = new QAction("Install a layout", this);
  layoutMenuLayoutsGroup = new QActionGroup(this);
  for (auto &layoutMenuLayout : layoutMenuLayouts) {
    layoutMenuLayout = new QAction(this);
    layoutMenuLayout->setVisible(false);
    layoutMenuLayout->setCheckable(true);
    layoutMenuLayoutsGroup->addAction(layoutMenuLayout);
    connect(layoutMenuLayout, SIGNAL(triggered()), this, SLOT(layoutMenuLayouts_clicked()));
  }
  RefreshLayouts();
  connect(layoutMenuInstall, SIGNAL(triggered()), this, SLOT(layoutMenuInstall_clicked()));

  // Icon Button Popup Menu
#if 0
  iconMenuOnTray = new QAction("Jump to system tray", this);
  connect(iconMenuOnTray, SIGNAL(triggered()), this, SLOT(iconMenuOnTray_clicked()));
#endif

  iconMenuLayout = new QAction("About current keyboard layout", this);
  connect(iconMenuLayout, SIGNAL(triggered()), this, SLOT(iconMenuLayout_clicked()));

  iconMenuAbout = new QAction("About OpenBangla Keyboard", this);
  connect(iconMenuAbout, SIGNAL(triggered()), this, SLOT(iconMenuAbout_clicked()));

  iconMenuUpdate = new QAction("Check for Updates", this);
  connect(iconMenuUpdate, &QAction::triggered, [=]() {
    checkForUpdate();
  });

  iconMenu = new QMenu(this);
  //iconMenu->addAction(iconMenuOnTray);
  iconMenu->addAction(iconMenuLayout);
  iconMenu->addAction(iconMenuAbout);
  iconMenu->addAction(iconMenuUpdate);  
}

void TopBar::SetupTrayIcon() {
#if 0
  /* TODO: Fix Crash... */
  tray = new QSystemTrayIcon(QIcon(":/images/keyboard_layout_viewer.png"), this);
  tray->setToolTip("OpenBangla Keyboard");

  /* Tray Menu */
  trayMenuRestore = new QAction("Restore TopBar", this);
  connect(trayMenuRestore, SIGNAL(triggered()), this, SLOT(trayMenuRestore_clicked()));

  trayMenu = new QMenu(this);
  trayMenu->addAction(trayMenuRestore);
  trayMenu->addMenu(layoutMenu); // Layout Menu
  /*
  trayMenu->addSeparator();
  trayMenu->addAction(iconMenuAbout);*/
  trayMenu->addSeparator();
  trayMenu->addAction(iconMenuQuit);

  tray->setContextMenu(trayMenu);
#endif
}

void TopBar::RefreshLayouts() {
  LayoutList list;
  list = gLayout->searchLayouts();

  QString selectedLayout = gSettings->getLayoutName();

  // This loop need to be rewritten to use `for each` loop, which is unnecessary. Skipping.


  for (int k = 0; k < MaxLayoutFiles; ++k) {
    if (k < list.count()) {
      QString name = list[k];
      layoutMenuLayouts[k]->setText(name);
      layoutMenuLayouts[k]->setVisible(true);
      // Select previously selected layout
      if (name == selectedLayout) {
        layoutMenuLayouts[k]->setChecked(true);
        gLayout->setLayout(name);
      }
    } else {
      layoutMenuLayouts[k]->setVisible(false);
    }
    layoutMenu->addAction(layoutMenuLayouts[k]);
  }
  layoutMenu->addSeparator();
  layoutMenu->addAction(layoutMenuInstall);
}

void TopBar::layoutMenuLayouts_clicked() {
  /**
   * From Qt version 5.10, Qt automatically adds shortcuts to
   * menu items. For that Qt includes a `&` character. So when
   * we use QAction::text() function, we get a string including
   * a `&` character and we mess all things up.
   *
   * See issue #17
   */

  QAction *action = qobject_cast<QAction *>(sender());

  QString layoutName = action->text();
  if (layoutName.contains("&")) {
    layoutName.replace("&", "");
  }

  gLayout->setLayout(layoutName);
  action->setChecked(true);
  layoutViewer->refreshLayoutViewer();
}

void TopBar::layoutMenuInstall_clicked() {
  QString fileName = QFileDialog::getOpenFileName(Q_NULLPTR, "Select Keyboard Layout", QDir::homePath(),
                                                  "Avro Keyboard 5 Keyboard Layout (*.avrolayout)");
  LayoutConverter conv;
  if (fileName.contains(".avrolayout") && fileName != "") {
    ConversionResult res = conv.convertAvroLayout(fileName);
    switch (res) {
    case Ok:
      QMessageBox::information(Q_NULLPTR, "OpenBangla Keyboard", "Layout Installed Successfully",
                               QMessageBox::Ok);
      break;
    case UnsupportedLayout:
      QMessageBox::critical(Q_NULLPTR,
                            "OpenBangla Keyboard",
                            "Unsupported Layout file!\nOpenBangla Keyboard only supports Avro Keyboard 5 layouts.",
                            QMessageBox::Ok);
      break;
    case OpenError:
      QMessageBox::critical(Q_NULLPTR, "OpenBangla Keyboard",
                            "An error occurred while opening the layout file!", QMessageBox::Ok);
      break;
    case SaveError:
      QMessageBox::critical(Q_NULLPTR, "OpenBangla Keyboard", "Error occurred while saving the file!",
                            QMessageBox::Ok);
      break;
    }
  }
  RefreshLayouts();
}

void TopBar::iconMenuLayout_clicked() {
  layoutViewer->showLayoutInfoDialog();
}

void TopBar::iconMenuAbout_clicked() {
  aboutDialog->show();
}

void TopBar::iconMenuOnTray_clicked() {
  this->setVisible(false);
  tray->setVisible(true);
  tray->showMessage("OpenBangla Keyboard", "OpenBangla Keyboard is now running on system tray");
}

void TopBar::trayMenuRestore_clicked() {
  tray->setVisible(false);
  this->setVisible(true);
}

void TopBar::on_buttonIcon_clicked() {
  // Check if this is not a position change event. If it is, then ignore it.
  if(!positionChanged) {
    QPoint point;
    point = this->pos();
    point.setX(point.x() + ui->buttonIcon->geometry().x());
    point.setY(point.y() + this->height());
    iconMenu->exec(point);
  }
}

void TopBar::closeEvent(QCloseEvent *event) {
  gSettings->setTopBarWindowPosition(this->pos());
  event->accept();
}

bool TopBar::eventFilter(QObject *object, QEvent *event) {
  if (object == ui->buttonIcon) {
    if (event->type() == QEvent::MouseButtonPress) {
      canMoveTopbar = true;
      positionChanged = false; // reset
      QMouseEvent *e = (QMouseEvent *) event;
      pressedMouseX = e->x();
      pressedMouseY = e->y();
      event->accept();
    } else if (event->type() == QEvent::MouseMove) {
      if (canMoveTopbar) {
        QMouseEvent *e = (QMouseEvent *) event;
        ui->buttonIcon->setCursor(Qt::ClosedHandCursor);
        move(e->globalX() - pressedMouseX, e->globalY() - pressedMouseY);
        positionChanged = true;
      }
    } else if (event->type() == QEvent::MouseButtonRelease) {
      canMoveTopbar = false;
      ui->buttonIcon->setCursor(Qt::ArrowCursor);
      event->accept();
    }
  }

  return QObject::eventFilter(object, event);
}

void TopBar::on_buttonSetLayout_clicked() {
  QPoint point;
  point = this->pos();
  point.setX(point.x() + ui->buttonSetLayout->geometry().x());
  point.setY(point.y() + this->height());
  layoutMenu->exec(point);
}

void TopBar::on_buttonShutdown_clicked() {
  QApplication::exit();
}

void TopBar::on_buttonViewLayout_clicked() {
  layoutViewer->refreshLayoutViewer();
  layoutViewer->show();
}

void TopBar::on_buttonSettings_clicked() {
  settingsDialog->updateSettings();
  settingsDialog->show();
}

/**
 * OBK from version 2 onwards reads and stores user data in a different directory
 * the following XDG specification.
 * This function checks and migrates data files into the new user data directory.
 **/
void TopBar::DataMigration() {
  UserFolders usr;
  LayoutConverter converter;
  if(gSettings->getPreviousUserDataRemains()) {
    QDir previousUserDataPath = QDir(environmentVariable("HOME", "") + "/.OpenBangla-Keyboard");
    if(previousUserDataPath.exists()) {
      // Handle the data files.
      migrateFile("phonetic-candidate-selection.json", previousUserDataPath, usr.dataPath());
      migrateFile("autocorrect.json", previousUserDataPath, usr.dataPath());
      // Convert old layout files if present.
      previousUserDataPath.cd("Layouts");
      QStringList list = previousUserDataPath.entryList(QStringList("*.json"));
      if(!list.empty()) {
        for(auto& file : list) {
          QString path = previousUserDataPath.path() + "/" + file;

          if(converter.convertLayoutFormat(path) != Ok) {
            QMessageBox::critical(Q_NULLPTR, "OpenBangla Keyboard",
                            QString("An error occurred while converting %1 layout!").arg(file), QMessageBox::Ok);
            return;
          }
        }
      }
      QMessageBox::information(Q_NULLPTR, "OpenBangla Keyboard", "User data files has been migrated successfully.",
                               QMessageBox::Ok);
      // Delete the previous user directory.
      previousUserDataPath.cdUp();
      previousUserDataPath.removeRecursively();
      gSettings->setPreviousUserDataRemains(false);
      RefreshLayouts();
    }
  }
}
