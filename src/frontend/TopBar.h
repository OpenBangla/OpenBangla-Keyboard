#ifndef TOPBAR_H
#define TOPBAR_H

#include <QMainWindow>

namespace Ui {
class TopBar;
}

class QAction;
class QMenu;

class TopBar : public QMainWindow
{
    Q_OBJECT

public:
    explicit TopBar(QWidget *parent = 0);
    ~TopBar();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void quitMenuQuit_clicked();

    void on_buttonAbout_clicked();

    void on_buttonSetLayout_clicked();

    void on_buttonShutdown_clicked();

private:
    Ui::TopBar *ui;
    bool canMoveTopbar;
    int pressedMouseX, pressedMouseY;

    QMenu *layoutMenu;
    QAction *layoutMenuInstall;
    QMenu *quitMenu;
    QAction *quitMenuQuit;

    void SetupTopBar();
    void SetupPopupMenus();
};

#endif // TOPBAR_H
