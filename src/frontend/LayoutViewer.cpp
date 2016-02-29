#include "LayoutViewer.h"
#include "Layout.h"
#include "Settings.h"
#include "AboutFile.h"
#include "ui_LayoutViewer.h"

LayoutViewer::LayoutViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LayoutViewer)
{
    ui->setupUi(this);
    aboutDialog = new AboutFile(this);
    this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
    this->move(gSettings->getLayoutViewerWindowPosition());
}

LayoutViewer::~LayoutViewer()
{
    delete aboutDialog;
    delete ui;
}

void LayoutViewer::refreshLayoutViewer() {
  desc = gLayout->getDesc();
  this->setWindowTitle(desc.name + " :: Layout Viewer");
  image.loadFromData(QByteArray::fromBase64(desc.image0));
  ui->labelImage->setPixmap(QPixmap::fromImage(image));
  ui->labelImage->adjustSize();
  this->setFixedHeight(ui->labelImage->height() + ui->labelImage->y());
  this->setFixedWidth(ui->labelImage->width());
}

void LayoutViewer::showLayoutInfoDialog() {
  aboutDialog->setDialogType(AboutLayout);
  aboutDialog->show();
}

void LayoutViewer::on_buttonClose_clicked()
{
  gSettings->setLayoutViewerWindowPosition(this->pos());
  LayoutViewer::close();
}

void LayoutViewer::on_buttonAboutLayout_clicked()
{
  showLayoutInfoDialog();
}
