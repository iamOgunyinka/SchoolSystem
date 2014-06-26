#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "existingstudent.h"
#include "showstudent.h"
#include "adminform.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    QMdiArea *workSpace;
private:
    QAction *addNewStudentAction, *checkResultAction,
            *adminAction,  *continueRegistrationAction, *exitAction;
    QMenu *fileMenu, *updateQueryMenu, *queryMenu;
    QToolBar *toolBar;
    Form *dialog;
    QString matric_number, oFileName;  //would be useful when calling ShowStudent in continueRegistration()
public:
    explicit MainWindow(QWidget *parent = 0);
    void createActions();
    void createMenus();
    void createToolbar();

public slots:
    void createNew();
    void admin();
    void closeEvent(QCloseEvent *);
    bool checkResult();
    void continueRegistration();
protected:
    bool isSafeToClose();
    void performAdminTasks();
signals:

};
#endif // MAINWINDOW_H
