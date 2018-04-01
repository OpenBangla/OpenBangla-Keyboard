#ifndef AUTOCORRECTDIALOG_H
#define AUTOCORRECTDIALOG_H

#include <QDialog>

namespace Ui {
class AutoCorrectDialog;
}

class AutoCorrectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutoCorrectDialog(QWidget *parent = 0);
    ~AutoCorrectDialog();

private:
    Ui::AutoCorrectDialog *ui;
};

#endif // AUTOCORRECTDIALOG_H
