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

#include <QCloseEvent>
#include <QResizeEvent>
#include <QLayout>
#include <QPushButton>
#include <QStyle>
#include <QFile>
#include <zstd.h>
#include "LayoutViewer.h"
#include "KeyboardWidget.h"
#include "Settings.h"
#include "AboutFile.h"
#include "base.hpp"
#include "ui_LayoutViewer.h"
#include "Log.h"

LayoutViewer::LayoutViewer(QString iconTheme, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LayoutViewer) {
  ui->setupUi(this);
  aboutDialog = new AboutFile(this);
  ui->buttonAboutLayout->setIcon(QIcon(":/images/" + iconTheme + "/info.svg"));
  ui->labelImage->setAlignment(Qt::AlignCenter);

  bool darkMode = (iconTheme == "white");
  QFile qss(darkMode ? ":/styles/dark.qss" : ":/styles/light.qss");
  if (qss.open(QFile::ReadOnly | QFile::Text)) {
    setStyleSheet(QString::fromUtf8(qss.readAll()));
  }

  this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
  this->move(gSettings->getLayoutViewerWindowPosition());

  // The info button floats above the content (it isn't in the layout); keep it
  // on top and give it a sane initial position so it doesn't flash at (0, 0).
  ui->buttonAboutLayout->raise();
  positionInfoButton();
}

LayoutViewer::~LayoutViewer() {
  delete aboutDialog;
  delete ui;
}

void LayoutViewer::refreshLayoutViewer() {
  image0.clear();
  image1.clear();
  ui->viewAltGr->setEnabled(false);
  ui->viewNormal->setEnabled(false);
  ui->labelImage->setText("");

  desc = gLayout->getDesc();
  this->setWindowTitle(desc.name + " :: Keyboard Layout Viewer");

  // Fixed layouts carry a per-key map, so we can draw the keyboard natively.
  QMap<QString, QString> keyMap;
  if (desc.type == Layout_Fixed) {
    keyMap = gLayout->getLayoutMap();
  }

  // The Normal/AltGr toggle only makes sense when the layout has a second view.
  // Phonetic (Avro) and Khipro layouts ship a single image, so the whole header
  // collapses and the info button floats over the content instead.
  bool twoViews = false;

  if (!keyMap.isEmpty()) {
    useKeyboard = true;
    ui->keyboard->setKeyMap(keyMap);
    ui->labelImage->hide();
    ui->keyboard->show();
    ui->viewNormal->setEnabled(true);
    twoViews = ui->keyboard->hasAltGr();
    ui->viewAltGr->setEnabled(twoViews);
    ui->headerBar->setVisible(twoViews);
    on_viewNormal_clicked();
  } else if (desc.image0.size() != 0) {
    // Phonetic / Khipro layouts ship an instructional image instead.
    useKeyboard = false;
    ui->keyboard->hide();
    ui->labelImage->show();
    image0 = decodeAndDecompress(desc.image0);
    if (desc.image1.size() != 0) {
      image1 = decodeAndDecompress(desc.image1);
      ui->viewAltGr->setEnabled(true);
      twoViews = true;
    }
    ui->viewNormal->setEnabled(true);
    ui->headerBar->setVisible(twoViews);
    on_viewNormal_clicked();
  } else {
    useKeyboard = false;
    ui->keyboard->hide();
    ui->labelImage->show();
    ui->headerBar->setVisible(false);
    ui->labelImage->setText("No image to display!");
    this->resize(537, 152);
  }

  // Use a more compact info button when it floats over a single-view image;
  // re-polish so the new padding (and its sizeHint) takes effect before we
  // position it.
  ui->buttonAboutLayout->setProperty("floating", !twoViews);
  ui->buttonAboutLayout->style()->unpolish(ui->buttonAboutLayout);
  ui->buttonAboutLayout->style()->polish(ui->buttonAboutLayout);

  // Re-pin the floating info button now that the header/content changed.
  positionInfoButton();

  // This refreshes Layout Info Dialog
  aboutDialog->setDialogType(AboutLayout);
}

void LayoutViewer::resizeEvent(QResizeEvent *event) {
  QDialog::resizeEvent(event);
  positionInfoButton();
}

void LayoutViewer::positionInfoButton() {
  // Flush any pending layout so the header/content geometries we read below are
  // current (e.g. right after the header was collapsed and adjustSize() ran).
  if (this->layout()) {
    this->layout()->activate();
  }

  QSize s = ui->buttonAboutLayout->sizeHint();
  ui->buttonAboutLayout->resize(s);

  int x, y;
  // isVisibleTo() (not isVisible()) so this is correct even while the dialog is
  // still hidden — refreshLayoutViewer() runs before show().
  if (ui->headerBar->isVisibleTo(this)) {
    // Two-view: sit at the header's top-right, vertically centered on it.
    QRect h = ui->headerBar->geometry();
    x = h.right() - s.width();
    y = h.top() + (h.height() - s.height()) / 2;
  } else {
    // Single-view: overlay the top-right corner of the content.
    QWidget *content = useKeyboard ? static_cast<QWidget *>(ui->keyboard)
                                   : static_cast<QWidget *>(ui->labelImage);
    QRect g = content->geometry();
    const int inset = 10;
    x = g.right() - s.width() - inset;
    y = g.top() + inset;
  }

  ui->buttonAboutLayout->move(x, y);
  ui->buttonAboutLayout->raise();
}

void LayoutViewer::showLayoutInfoDialog() {
  aboutDialog->setDialogType(AboutLayout);
  aboutDialog->show();
}

void LayoutViewer::closeEvent(QCloseEvent *event) {
  gSettings->setLayoutViewerWindowPosition(this->pos());
  this->hide();
  event->ignore();
}

void LayoutViewer::on_buttonAboutLayout_clicked() {
  showLayoutInfoDialog();
}

void LayoutViewer::on_viewNormal_clicked() {
  if (useKeyboard) {
    ui->keyboard->setMode(KeyboardWidget::Normal);
  } else {
    image.loadFromData(image0);
    ui->labelImage->setPixmap(QPixmap::fromImage(image));
    ui->labelImage->adjustSize();
  }
  ui->viewNormal->setChecked(true);
  // Let the layout (margins + the active widget's size hint) drive the size.
  this->adjustSize();
}

void LayoutViewer::on_viewAltGr_clicked() {
  if (useKeyboard) {
    ui->keyboard->setMode(KeyboardWidget::AltGr);
  } else {
    image.loadFromData(image1);
    ui->labelImage->setPixmap(QPixmap::fromImage(image));
    ui->labelImage->adjustSize();
  }
  ui->viewAltGr->setChecked(true);
  this->adjustSize();
}

QByteArray LayoutViewer::decodeAndDecompress(QByteArray &data) {
  std::string decoded = base91::decode(std::string(data.data(), data.size()));
  unsigned long long cap = ZSTD_getFrameContentSize(decoded.data(), decoded.size());

  if(cap == ZSTD_CONTENTSIZE_ERROR) {
    LOG_ERROR("Layout Image: Not compressed by ZSTD!");
    return QByteArray(decoded.data(), decoded.size());
  } else if(cap == ZSTD_CONTENTSIZE_UNKNOWN) {
    LOG_ERROR("Layout Image: Unknown decompressed size!");
    return QByteArray(decoded.data(), decoded.size());
  }
  
  char *imgData = (char *)malloc(cap);

  size_t decompressed = ZSTD_decompress(imgData, cap, decoded.data(), decoded.size());
  QByteArray img = QByteArray(imgData, decompressed);
  free(imgData);

  return img;
}
