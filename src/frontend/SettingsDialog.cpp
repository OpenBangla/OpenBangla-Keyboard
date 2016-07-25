#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "Settings.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    this->setFixedSize(QSize(this->width(), this->height()));
    updateSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::updateSettings() {
    ui->checkBoxPrevWin->setChecked(gSettings->getEnterKeyClosesPrevWin());
    ui->checkBoxCandidateWinHorizontal->setChecked(gSettings->getCandidateWinHorizontal());
}

void SettingsDialog::on_buttonBox_accepted()
{
    gSettings->setEnterKeyClosesPrevWin(ui->checkBoxPrevWin->isChecked());
    gSettings->setCandidateWinHorizontal(ui->checkBoxCandidateWinHorizontal->isChecked());
}

void SettingsDialog::on_buttonBox_rejected()
{
    SettingsDialog::close();
}
