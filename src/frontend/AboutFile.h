#ifndef ABOUTFILE_H
#define ABOUTFILE_H

#include <QDialog>
#include "Layout.h"

namespace Ui {
class AboutFile;
}

enum DialogType {
  AboutLayout
};

class AboutFile : public QDialog
{
    Q_OBJECT

public:
    explicit AboutFile(QWidget *parent = 0);
    ~AboutFile();

    void setDialogType(DialogType type);

private slots:
    void on_buttonOK_clicked();

private:
    Ui::AboutFile *ui;

    void loadLayoutInfo();
};

#endif // ABOUTFILE_H
