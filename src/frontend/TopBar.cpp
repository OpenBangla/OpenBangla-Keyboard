/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2016-2022 Muhammad Mominul Huque <mominul2082@gmail.com>
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
#include <QScreen>
#include <QMessageBox>
#include <QMouseEvent>
#include <QFileDialog>
#include <QPushButton>
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
#include "PlatformConfig.h"
#include "ui_TopBar.h"


TopBar::TopBar(bool darkIcon, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::TopBar) {
  ui->setupUi(this);

  gLayout = new Layout();

  if(darkIcon) {
    m_iconTheme = "black";
  } else {
    m_iconTheme = "white";
  }

  /* Dialogs */
  aboutDialog = new AboutDialog(Q_NULLPTR);
  layoutViewer = new LayoutViewer(m_iconTheme, Q_NULLPTR);
  settingsDialog = new SettingsDialog(Q_NULLPTR);

  // Update tray state after settings have been changed.
  connect(settingsDialog, &QDialog::finished, [&]() {
    trayOutputModeUnicode->setChecked(true);
    trayOutputModeANSI->setChecked(gSettings->getANSIEncoding());
  });

  auto set_icon = [&](QPushButton* obj, QString icon) {
    obj->setIcon(QIcon(":/images/" + m_iconTheme + "/" + icon + ".svg"));
  };
  set_icon(ui->buttonSetLayout, "layouts");
  set_icon(ui->buttonViewLayout, "layout");
  set_icon(ui->buttonSettings, "settings");
  set_icon(ui->buttonShutdown, "close");
  ui->buttonIcon->installEventFilter(this);

  SetupTopBar();
  SetupPopupMenus();
  SetupTrayIcon();
  DataMigration();

  uint count = gSettings->getTrayInfoCount();
  if(count < 4 && !gSettings->getTopBarVisibility()) {
    tray->showMessage("OpenBangla Keyboard", "Currently running in the system tray.\n"
                      "You can use the tray icon to change keyboard layouts and other "
                      "settings and to show the TopBar again.");
    // Update the counter to show only the message for the first three times
    gSettings->setTrayInfoCount(count + 1);
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
  this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  this->setFixedSize(QSize(this->width(), this->height()));
  this->setAttribute(Qt::WA_TranslucentBackground);

  if (gSettings->getTopBarWindowPosition() == QPoint(0, 0)) {
    int width = this->frameGeometry().width();
    int height = this->frameGeometry().height();

    QApplication *app = (QApplication *) QApplication::instance();
    QScreen *screen = app->primaryScreen();

    int screenWidth = screen->geometry().width();
    int screenHeight = screen->geometry().height();

    this->setGeometry((screenWidth / 2) - (width / 2), (screenHeight / 2) - (height / 2), width, height);
  } else {
    move(gSettings->getTopBarWindowPosition());
  }
}

void TopBar::SetupPopupMenus() {
  // Layout Popup Menu
  layoutMenu = new QMenu("Select a keyboard layout", this);
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
  iconMenuHide = new QAction("Hide this TopBar", this);
  connect(iconMenuHide, &QAction::triggered, [&]() {
    this->setVisible(false);
    trayTopBarVisibility->setText("Show the TopBar");
  });

  iconMenuLayout = new QAction("About current keyboard layout", this);
  connect(iconMenuLayout, SIGNAL(triggered()), this, SLOT(iconMenuLayout_clicked()));

  iconMenuAbout = new QAction("About OpenBangla Keyboard", this);
  connect(iconMenuAbout, SIGNAL(triggered()), this, SLOT(iconMenuAbout_clicked()));

  iconMenu = new QMenu(this);
  // Gnome Shell and Pantheon doesn't support tray icon natively.
  if(shouldShowTrayIcon()) {
    iconMenu->addAction(iconMenuHide);
  }
  iconMenu->addAction(iconMenuLayout);
  iconMenu->addAction(iconMenuAbout);
}

void TopBar::SetupTrayIcon() {
  tray = new QSystemTrayIcon(QIcon::fromTheme("openbangla-keyboard", QIcon(":/images/tray.png")), this);
  tray->setToolTip("OpenBangla Keyboard");

  /* Tray Menu */
  trayLayoutViewer = new QAction("Layout Viewer", this);
  connect(trayLayoutViewer, &QAction::triggered, this, &TopBar::on_buttonViewLayout_clicked);

  /* Output Mode Menu */
  trayOutputMode = new QMenu("Output Mode", this);
  trayOutputModeUnicode = new QAction("Unicode", this);
  trayOutputModeANSI = new QAction("ANSI", this);
  trayOutputModeGroup = new QActionGroup(this);
  trayOutputModeUnicode->setCheckable(true);
  trayOutputModeANSI->setCheckable(true);
  trayOutputModeGroup->addAction(trayOutputModeUnicode);
  trayOutputModeGroup->addAction(trayOutputModeANSI);
  trayOutputMode->addActions(trayOutputModeGroup->actions());
  trayOutputModeUnicode->setChecked(true);
  trayOutputModeANSI->setChecked(gSettings->getANSIEncoding());
  connect(trayOutputModeGroup, &QActionGroup::triggered, this, [=]() {
    gSettings->setANSIEncoding(trayOutputModeANSI->isChecked());
  });

  traySettings = new QAction("Settings", this);
  connect(traySettings, &QAction::triggered, this, &TopBar::on_buttonSettings_clicked);

  trayTopBarVisibility = new QAction(
    gSettings->getTopBarVisibility() ? "Hide the TopBar" : "Show the TopBar",
    this
  );

  connect(trayTopBarVisibility, &QAction::triggered, [&]() {
    if(this->isVisible()) {
      this->setVisible(false);
      trayTopBarVisibility->setText("Show the TopBar");
    } else {
      this->setVisible(true);
      trayTopBarVisibility->setText("Hide the TopBar");
    }
  });

  trayQuit = new QAction("Quit", this);
  connect(trayQuit, &QAction::triggered, this, &TopBar::on_buttonShutdown_clicked);

  trayMenu = new QMenu(this);
  trayMenu->addMenu(layoutMenu); // Layout Menu
  trayMenu->addMenu(trayOutputMode);
  trayMenu->addAction(trayLayoutViewer);
  trayMenu->addAction(traySettings);
  trayMenu->addSeparator();
  trayMenu->addAction(trayTopBarVisibility);
  trayMenu->addAction(trayQuit);

  tray->setContextMenu(trayMenu);
  tray->setVisible(true);
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
                     "All Supported Layouts (*.avrolayout *.json);;Avro Keyboard 5 Keyboard Layout (*.avrolayout);;OpenBangla Keyboard Layout (*.json)");
  LayoutConverter conv;
  if (fileName.endsWith(".avrolayout")) {
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
  } else if (fileName.endsWith(".json")) {
    ConversionResult res = conv.saveLayout(fileName);
    switch (res) {
    case Ok:
      QMessageBox::information(Q_NULLPTR, "OpenBangla Keyboard", "Layout Installed Successfully",
                               QMessageBox::Ok);
      break;
    case UnsupportedLayout:
      QMessageBox::critical(Q_NULLPTR, "OpenBangla Keyboard", "Unsupported Layout file!",
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
  gSettings->setTopBarVisibility(this->isVisible());
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
  gSettings->setTopBarWindowPosition(this->pos());
  gSettings->setTopBarVisibility(this->isVisible());
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
