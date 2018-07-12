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
#include "TopBar.h"
#include "Layout.h"
#include "Settings.h"
#include "LayoutViewer.h"
#include "AboutDialog.h"
#include "AboutFile.h"
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
    autoCorrectDialog = new AutoCorrectDialog(Q_NULLPTR);

    ui->buttonIcon->installEventFilter(this);

    SetupTopBar();
    SetupPopupMenus();
    SetupTrayIcon();

    if (gSettings->getUpdateCheck()) {
        checkForUpdate();
    }
}

TopBar::~TopBar() {
    /* Dialogs */
    delete layoutViewer;
    delete settingsDialog;
    delete aboutDialog;
    delete autoCorrectDialog;

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
    layoutMenuInstall = new QAction("Install a layout", this);
    layoutMenuLayoutsGroup = new QActionGroup(this);
    for (int i = 0; i < MaxLayoutFiles; ++i) {
        layoutMenuLayouts[i] = new QAction(this);
        layoutMenuLayouts[i]->setVisible(false);
        layoutMenuLayouts[i]->setCheckable(true);
        layoutMenuLayoutsGroup->addAction(layoutMenuLayouts[i]);
        connect(layoutMenuLayouts[i], SIGNAL(triggered()), this, SLOT(layoutMenuLayouts_clicked()));
    }
    RefreshLayouts();
    connect(layoutMenuInstall, SIGNAL(triggered()), this, SLOT(layoutMenuInstall_clicked()));

    // Settings Popup Menu
    settingsMenuFixedLayoutAutoVForm = new QAction("Enable \"Automatic Vowel Forming\"", this);
    settingsMenuFixedLayoutAutoVForm->setCheckable(true);
    settingsMenuFixedLayoutAutoVForm->setChecked(gSettings->getAutoVowelFormFixed());
    connect(settingsMenuFixedLayoutAutoVForm, SIGNAL(triggered()), this,
            SLOT(settingsMenuFixedLayoutAutoVForm_clicked()));

    settingsMenuFixedLayoutAutoChandra = new QAction("Automatically fix \"Chandrabindu\" position", this);
    settingsMenuFixedLayoutAutoChandra->setCheckable(true);
    settingsMenuFixedLayoutAutoChandra->setChecked(gSettings->getAutoChandraPosFixed());
    connect(settingsMenuFixedLayoutAutoChandra, SIGNAL(triggered()), this,
            SLOT(settingsMenuFixedLayoutAutoChandra_clicked()));

    settingsMenuFixedLayoutOldReph = new QAction("Old Style Reph", this);
    settingsMenuFixedLayoutOldReph->setCheckable(true);
    settingsMenuFixedLayoutOldReph->setChecked(gSettings->getOldReph());
    connect(settingsMenuFixedLayoutOldReph, SIGNAL(triggered()), this, SLOT(settingsMenuFixedLayoutOldReph_clicked()));

    settingsMenuFixedLayoutTraditionalKar = new QAction("Enable \"Traditional Kar Joining\"", this);
    settingsMenuFixedLayoutTraditionalKar->setCheckable(true);
    settingsMenuFixedLayoutTraditionalKar->setChecked(gSettings->getTraditionalKarFixed());
    connect(settingsMenuFixedLayoutTraditionalKar, SIGNAL(triggered()), this,
            SLOT(settingsMenuFixedLayoutTraditionalKar_clicked()));

    settingsMenuFixedLayoutNumberPad = new QAction("Enable Bengali in NumberPad", this);
    settingsMenuFixedLayoutNumberPad->setCheckable(true);
    settingsMenuFixedLayoutNumberPad->setChecked(gSettings->getNumberPadFixed());
    connect(settingsMenuFixedLayoutNumberPad, SIGNAL(triggered()), this,
            SLOT(settingsMenuFixedLayoutNumberPad_clicked()));

    settingsMenuFixedLayout = new QMenu("Fixed Keyboard Layout Options", this);
    settingsMenuFixedLayout->addAction(settingsMenuFixedLayoutAutoVForm);
    settingsMenuFixedLayout->addAction(settingsMenuFixedLayoutAutoChandra);
    settingsMenuFixedLayout->addAction(settingsMenuFixedLayoutOldReph);
    settingsMenuFixedLayout->addAction(settingsMenuFixedLayoutTraditionalKar);
    settingsMenuFixedLayout->addAction(settingsMenuFixedLayoutNumberPad);

    settingsMenuShowDialog = new QAction("Settings", this);
    connect(settingsMenuShowDialog, SIGNAL(triggered()), this, SLOT(settingsMenuShowDialog_clicked()));

    settingsMenuAutoCorrect = new QAction("Edit Phonetic AutoCorrect entries", this);
    connect(settingsMenuAutoCorrect, SIGNAL(triggered()), this, SLOT(settingsMenuAutoCorrect_clicked()));

    settingsMenu = new QMenu(this);
    settingsMenu->addMenu(settingsMenuFixedLayout);
    settingsMenu->addAction(settingsMenuAutoCorrect);
    settingsMenu->addSeparator();
    settingsMenu->addAction(settingsMenuShowDialog);

    // About Popup Menu
    aboutMenuLayout = new QAction("About current keyboard layout", this);
    connect(aboutMenuLayout, SIGNAL(triggered()), this, SLOT(aboutMenuLayout_clicked()));

    aboutMenuAbout = new QAction("About OpenBangla Keyboard", this);
    connect(aboutMenuAbout, SIGNAL(triggered()), this, SLOT(aboutMenuAbout_clicked()));

    aboutMenuUpdate = new QAction("Check for Updates", this);
    connect(aboutMenuUpdate, &QAction::triggered, [=]() {
        checkForUpdate();
    });

    aboutMenu = new QMenu(this);
    aboutMenu->addAction(aboutMenuLayout);
    aboutMenu->addAction(aboutMenuAbout);
    aboutMenu->addAction(aboutMenuUpdate);

    // Quit Popup Menu
    quitMenuQuit = new QAction("Quit", this);
    connect(quitMenuQuit, SIGNAL(triggered()), this, SLOT(quitMenuQuit_clicked()));

#if 0
    quitMenuOnTray = new QAction("Jump to system tray", this);
    connect(quitMenuOnTray, SIGNAL(triggered()), this, SLOT(quitMenuOnTray_clicked()));
#endif

    quitMenu = new QMenu(this);
    //quitMenu->addAction(quitMenuOnTray);
    quitMenu->addAction(quitMenuQuit);
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
    trayMenu->addAction(aboutMenuAbout);*/
    trayMenu->addSeparator();
    trayMenu->addAction(quitMenuQuit);

    tray->setContextMenu(trayMenu);
#endif
}

void TopBar::RefreshLayouts() {
    LayoutList list;
    list = gLayout->searchLayouts();

    QString selectedLayout = gSettings->getLayoutName();

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
        ConversionResult res = conv.convertLayout(fileName);
        switch (res) {
            case Ok:
                QMessageBox::information(Q_NULLPTR, "OpenBangla Keyboard", "Layout Installed Successfully",
                                         QMessageBox::Ok);
                break;
            case UnsupportedLayout:
                QMessageBox::critical(Q_NULLPTR, "OpenBangla Keyboard",
                                      "Unsupported Layout file!\nOpenBangla Keyboard only supports Avro Keyboard 5 layouts.",
                                      QMessageBox::Ok);
                break;
            case OpenError:
                QMessageBox::critical(Q_NULLPTR, "OpenBangla Keyboard",
                                      "An error occured when opening the layout file!", QMessageBox::Ok);
                break;
            case SaveError:
                QMessageBox::critical(Q_NULLPTR, "OpenBangla Keyboard", "Error occured when saving the file!",
                                      QMessageBox::Ok);
                break;
        }
    }
    RefreshLayouts();
}

void TopBar::settingsMenuFixedLayoutAutoVForm_clicked() {
    gSettings->setAutoVowelFormFixed(settingsMenuFixedLayoutAutoVForm->isChecked());
}

void TopBar::settingsMenuFixedLayoutAutoChandra_clicked() {
    gSettings->setAutoChandraPosFixed(settingsMenuFixedLayoutAutoChandra->isChecked());
}

void TopBar::settingsMenuFixedLayoutOldReph_clicked() {
    gSettings->setOldReph(settingsMenuFixedLayoutOldReph->isChecked());
}

void TopBar::settingsMenuFixedLayoutTraditionalKar_clicked() {
    gSettings->setTraditionalKarFixed(settingsMenuFixedLayoutTraditionalKar->isChecked());
}

void TopBar::settingsMenuFixedLayoutNumberPad_clicked() {
    gSettings->setNumberPadFixed(settingsMenuFixedLayoutNumberPad->isChecked());
}

void TopBar::settingsMenuAutoCorrect_clicked() {
    autoCorrectDialog->show();
}

void TopBar::settingsMenuShowDialog_clicked() {
    settingsDialog->updateSettings();
    settingsDialog->show();
}

void TopBar::aboutMenuLayout_clicked() {
    layoutViewer->showLayoutInfoDialog();
}

void TopBar::aboutMenuAbout_clicked() {
    aboutDialog->show();
}

void TopBar::quitMenuOnTray_clicked() {
    this->setVisible(false);
    tray->setVisible(true);
    tray->showMessage("OpenBangla Keyboard", "OpenBangla Keyboard is now running on system tray");
}

void TopBar::quitMenuQuit_clicked() {
    TopBar::close();
}

void TopBar::trayMenuRestore_clicked() {
    tray->setVisible(false);
    this->setVisible(true);
}

void TopBar::on_buttonAbout_clicked() {
    QPoint point;
    point = this->pos();
    point.setX(point.x() + ui->buttonAbout->geometry().x());
    point.setY(point.y() + this->height());
    aboutMenu->exec(point);
}

void TopBar::closeEvent(QCloseEvent *event) {
    gSettings->setTopBarWindowPosition(this->pos());
    event->accept();
}


bool TopBar::eventFilter(QObject *object, QEvent *event) {
    if (object == ui->buttonIcon) {
        if (event->type() == QEvent::MouseButtonPress) {
            canMoveTopbar = true;
            QMouseEvent *e = (QMouseEvent *) event;
            pressedMouseX = e->x();
            pressedMouseY = e->y();
            event->accept();
        } else if (event->type() == QEvent::MouseMove) {
            if (canMoveTopbar) {
                QMouseEvent *e = (QMouseEvent *) event;
                ui->buttonIcon->setCursor(Qt::ClosedHandCursor);
                move(e->globalX() - pressedMouseX, e->globalY() - pressedMouseY);
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            canMoveTopbar = false;
            ui->buttonIcon->setCursor(Qt::OpenHandCursor);
            event->accept();
        } else if (event->type() == QEvent::Enter) {
            ui->buttonIcon->setCursor(Qt::OpenHandCursor);
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
    QPoint point;
    point = this->pos();
    point.setX(point.x() + ui->buttonShutdown->geometry().x());
    point.setY(point.y() + this->height());
    quitMenu->exec(point);
}

void TopBar::on_buttonViewLayout_clicked() {
    layoutViewer->refreshLayoutViewer();
    layoutViewer->show();
}

void TopBar::on_buttonSettings_clicked() {
    QPoint point;
    point = this->pos();
    point.setX(point.x() + ui->buttonSettings->geometry().x());
    point.setY(point.y() + this->height());
    settingsMenu->exec(point);
}
