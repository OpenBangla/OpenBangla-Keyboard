#include <QMessageBox>
#include <QMouseEvent>
#include "TopBar.h"
#include "ui_TopBar.h"

TopBar::TopBar(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TopBar)
{
    ui->setupUi(this);

    //connect(ui->buttonIcon, SIGNAL(mouseMoveEvent(QMouseEvent*)), this, SLOT(mouseMoveButtonIcon(QMouseEvent*)));
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
}

TopBar::~TopBar()
{
    delete ui;
}

void TopBar::on_buttonAbout_clicked()
{
    QMessageBox::about(this, "About OpenBangla Keyboard",
        tr("<h2>OpenBangla Keyboard 0.1</h2>"
        "<p>Copyright &copy; 2015-2016 Muhammad Mominul Huque & OpenBangla Team."
        "<p>An OpenSource, Cross-Platform, Unicode Compliant Bengali Input Method."));
}

void TopBar::mouseMoveEvent(QMouseEvent *event) {
  move(event->globalX() - pressedMouseX, event->globalY() - pressedMouseY);
}

void TopBar::mousePressEvent(QMouseEvent *event) {
  pressedMouseX = event->x();
  pressedMouseY = event->y();
  event->accept();
}

void TopBar::on_buttonIcon_clicked()
{
    TopBar::close();
}
