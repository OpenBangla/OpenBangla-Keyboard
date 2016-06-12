#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class QTimer;

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void scroll();

    void on_btnClose_clicked();

    void on_btnLicense_toggled(bool checked);

    void on_labelDesc_linkActivated(const QString &link);

private:
    Ui::AboutDialog *ui;

    QTimer *timer;
};

#endif // ABOUTDIALOG_H
