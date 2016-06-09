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
    timer->setInterval(30);
    timer->start();

    ui->txtLicense->setVisible(false);
    ui->labelDesc->installEventFilter(this);
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

bool AboutDialog::eventFilter(QObject *object, QEvent *event) {
    if (object == ui->labelDesc) {
        switch (event->type()) {
            case QEvent::Enter:
                timer->stop();
                break;
            case QEvent::Leave:
                timer->start();
                break;
            default:
                break;
        }
    }
    return QObject::eventFilter(object, event);
}
