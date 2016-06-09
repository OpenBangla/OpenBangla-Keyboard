#include <QTimer>
#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    this->setFixedSize(this->width(),this->height());

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(scroll()));
    timer->start(30);

    ui->txtLicense->setVisible(false);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::scroll() {
    QPoint point;
    point.setX(ui->labelDesc->x());
    if(ui->labelDesc->y() != -610) {
        int y = ui->labelDesc->y() - 1;
        point.setY(y);
        ui->labelDesc->move(point);
    } else {
        point.setY(300);
        ui->labelDesc->move(point);
    }
}

void AboutDialog::on_btnClose_clicked()
{
    this->close();
}

void AboutDialog::on_btnLicense_toggled(bool checked)
{
    ui->txtLicense->setVisible(checked);
    ui->labelDesc->setVisible(!checked);
}
