#ifndef TOPBAR_H
#define TOPBAR_H

#include <QMainWindow>

namespace Ui {
class TopBar;
}

class TopBar : public QMainWindow
{
    Q_OBJECT

public:
    explicit TopBar(QWidget *parent = 0);
    ~TopBar();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void on_buttonAbout_clicked();
    void on_buttonIcon_clicked();

private:
    Ui::TopBar *ui;
    int pressedMouseX, pressedMouseY;
};

#endif // TOPBAR_H
