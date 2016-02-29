#ifndef TOPBAR_H
#define TOPBAR_H

#include <QMainWindow>

namespace Ui {
class TopBar;
}

class QActionGroup;
class QAction;
class QMenu;

class LayoutViewer;
class AboutFile;

class TopBar : public QMainWindow
{
    Q_OBJECT

public:
    explicit TopBar(QWidget *parent = 0);
    ~TopBar();

protected:
    void closeEvent(QCloseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void layoutMenuLayouts_clicked();
    void aboutMenuLayout_clicked();
    void aboutMenuAbout_clicked();
    void quitMenuQuit_clicked();

    void on_buttonAbout_clicked();

    void on_buttonSetLayout_clicked();

    void on_buttonShutdown_clicked();

    void on_buttonViewLayout_clicked();

private:
    Ui::TopBar *ui;
    bool canMoveTopbar;
    int pressedMouseX, pressedMouseY;

    /* Dialogs */
    LayoutViewer *layoutViewer;

    QMenu *layoutMenu;
    enum { MaxLayoutFiles = 10 };
    QAction *layoutMenuLayouts[MaxLayoutFiles];
    QActionGroup *layoutMenuLayoutsGroup;
    QAction *layoutMenuInstall;
    QMenu *aboutMenu;
    QAction *aboutMenuLayout;
    QAction *aboutMenuAbout;
    QMenu *quitMenu;
    QAction *quitMenuQuit;

    void SetupTopBar();
    void SetupPopupMenus();

    void RefreshLayouts();
};

#endif // TOPBAR_H
