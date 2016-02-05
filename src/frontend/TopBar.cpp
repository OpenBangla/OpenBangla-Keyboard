#include "TopBar.h"
#include "ui_TopBar.h"

TopBar::TopBar(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TopBar)
{
    ui->setupUi(this);
}

TopBar::~TopBar()
{
    delete ui;
}
