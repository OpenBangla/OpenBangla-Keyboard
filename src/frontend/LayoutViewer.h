#ifndef LAYOUTVIEWER_H
#define LAYOUTVIEWER_H

#include <QDialog>
#include "Layout.h"

namespace Ui {
class LayoutViewer;
}

class QImage;
class AboutFile;

class LayoutViewer : public QDialog
{
    Q_OBJECT

public:
    explicit LayoutViewer(QWidget *parent = 0);
    ~LayoutViewer();

    void refreshLayoutViewer();
    void showLayoutInfoDialog();

private slots:
    void on_buttonClose_clicked();

    void on_buttonAboutLayout_clicked();

private:
    Ui::LayoutViewer *ui;

    AboutFile *aboutDialog;
    QImage image;
    LayoutDesc desc;
};

#endif // LAYOUTVIEWER_H
