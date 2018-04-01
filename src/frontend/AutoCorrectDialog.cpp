#include "AutoCorrectDialog.h"
#include "ui_AutoCorrectDialog.h"

AutoCorrectDialog::AutoCorrectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoCorrectDialog)
{
    ui->setupUi(this);
}

AutoCorrectDialog::~AutoCorrectDialog()
{
    delete ui;
}
