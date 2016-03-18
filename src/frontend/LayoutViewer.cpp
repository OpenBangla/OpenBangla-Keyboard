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
    ui->labelImage->setAlignment(Qt::AlignCenter);
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
  on_viewNormal_clicked();
  // This refreshes Layout Info Dialog
  aboutDialog->setDialogType(AboutLayout);
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

void LayoutViewer::on_viewNormal_clicked()
{
    ui->labelImage->setText("");
    image.loadFromData(QByteArray::fromBase64(desc.image0));
    ui->labelImage->setPixmap(QPixmap::fromImage(image));
    ui->labelImage->adjustSize();
    this->setFixedHeight(ui->labelImage->height() + ui->labelImage->y());
    this->setFixedWidth(ui->labelImage->width());
    ui->viewNormal->setChecked(true);
}

void LayoutViewer::on_viewAltGr_clicked()
{
    if(desc.image1.size() != 0) {
        image.loadFromData(QByteArray::fromBase64(desc.image1));
        ui->labelImage->setPixmap(QPixmap::fromImage(image));
        ui->labelImage->adjustSize();
        this->setFixedHeight(ui->labelImage->height() + ui->labelImage->y());
        this->setFixedWidth(ui->labelImage->width());
        ui->viewAltGr->setChecked(true);
    } else {
        ui->labelImage->setText("No image to display!");
    }
}
