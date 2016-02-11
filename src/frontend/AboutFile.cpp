#include "AboutFile.h"
#include "ui_AboutFile.h"

AboutFile::AboutFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutFile)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
    this->setFixedSize(QSize(this->width(), this->height()));
}

AboutFile::~AboutFile()
{
    delete ui;
}

void AboutFile::setDialogType(DialogType type) {
  if(type == AboutLayout) {
    this->setWindowTitle("About This Layout...");
    loadLayoutInfo();
  }
}

void AboutFile::loadLayoutInfo() {
  LayoutDesc desc = gLayout->getDesc();
  ui->txtName->setText(desc.name);
  ui->txtVar->setText(desc.ver);
  ui->txtDevName->setText(desc.devName);
  ui->txtDevComment->setPlainText(desc.devComment);
}

void AboutFile::on_buttonOK_clicked()
{
    AboutFile::close();
}
