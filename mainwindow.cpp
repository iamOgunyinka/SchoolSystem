#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), workSpace(new QMdiArea),
    addNewStudentAction(nullptr),
    checkResultAction(nullptr), adminAction(nullptr), continueRegistrationAction(nullptr),
    exitAction(nullptr), fileMenu(new QMenu), updateQueryMenu(new QMenu),
    queryMenu(new QMenu), toolBar(new QToolBar), dialog(nullptr)
{
    //setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QIcon(tr(":/images/gedit-logo.png")));
    showMaximized();
    setWindowTitle(tr("%1").arg("Student Registration Manager"));

    setCentralWidget(workSpace);
    createActions();
    createMenus();
    createToolbar();

    statusBar()->showMessage("Done");
}

void MainWindow::createActions()
{
    addNewStudentAction = new QAction(QIcon(":/images/new.png"), tr("&New Student"), this);
    addNewStudentAction->setShortcut(QKeySequence {"Ctrl+N"});
    addNewStudentAction->setStatusTip(tr("Create a new student record in the database"));
    QObject::connect(addNewStudentAction, SIGNAL(triggered()), this, SLOT(createNew()));

    checkResultAction = new QAction(QIcon(":/images/open.png"), tr("Chec&k Result"), this);
    checkResultAction->setShortcut(tr("Ctrl+D"));
    checkResultAction->setStatusTip("Check result");
    QObject::connect(checkResultAction, SIGNAL(triggered()), this, SLOT(checkResult()));

    adminAction = new QAction(QIcon(":/images/admin.png"), tr("&Administrative Tasks"), this);
    adminAction->setShortcut(tr("Ctrl+Q"));
    adminAction->setStatusTip(tr("Perform administrator's task"));
    QObject::connect(adminAction, SIGNAL(triggered()), this, SLOT(admin()));

    continueRegistrationAction = new QAction(QIcon(":images/continue.svg"), "Continue Registration", this);
    continueRegistrationAction->setShortcut(tr("Ctrl+R"));
    continueRegistrationAction->setStatusTip("Continue Registration for new students");
    QObject::connect(continueRegistrationAction, SIGNAL(triggered()), this, SLOT(continueRegistration()));

    exitAction = new QAction(QIcon(":/images/_exit.png"), tr("Exit"), this);
    exitAction->setShortcut(tr("Ctrl+W"));
    QObject::connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu("&Student");
    fileMenu->addAction(addNewStudentAction);
    fileMenu->addAction(continueRegistrationAction);
    //fileMenu->addAction(checkResultAction);
    fileMenu->addAction(exitAction);

    updateQueryMenu = menuBar()->addMenu("&Admin");
    updateQueryMenu->addAction(adminAction);
}

void MainWindow::createToolbar()
{
    toolBar = addToolBar("ToolBar");
    toolBar->addAction(addNewStudentAction);
    toolBar->addAction(continueRegistrationAction);
    //toolBar->addAction(checkResultAction);
    toolBar->addAction(adminAction);
    toolBar->addAction(exitAction);
}

void MainWindow::createNew()
{
    dialog = new Form(oFileName, this);
    workSpace->addSubWindow(dialog);
    dialog->setWindowTitle("Create New Record");
    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->show();
}

void MainWindow::admin()
{
    LoginDialog login;
    if(login.exec() == QDialog::Accepted){
        QString username = login.getUsername(), password = login.getPassword();
        if(login.loginAdmin(username, password)){
            performAdminTasks();
        } else {
            QMessageBox::information(this, "Admin", "Invalid username/password combination",
                                     QMessageBox::Ok);
        }
    } else {
        QMessageBox::information(this, "Admin", "Login cancelled", QMessageBox::Ok);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(isSafeToClose()){
        QMessageBox::information(this, windowTitle(), tr("Thank you for using %1! Good-Bye")
                                 .arg(windowTitle()), QMessageBox::Ok);
        event->accept();
    } else {
        QMessageBox::information(this, windowTitle(), "Please make sure all Sub-Windows are closed first",
                                 QMessageBox::Ok);
        event->ignore();
    }
}

bool MainWindow::isSafeToClose()
{
    if(workSpace->subWindowList().size() == 0){
        return true;
    } else {
        return false;
    }
}

bool MainWindow::checkResult()
{
    LoginDialog login;
    if(login.exec() == QDialog::Accepted){
        matric_number = login.getUsername();
        if(login.checkLogin(matric_number, login.getPassword())){
            QMessageBox::information(nullptr, "Login", "Login Successful", QMessageBox::Ok);
            return true;
        } else {
            QMessageBox::critical(nullptr, "Login", "Invalid username or password", QMessageBox::Ok);
            return false;
        }
    }
    return false;
}

void MainWindow::continueRegistration()
{
    if(checkResult()){
        ShowStudent *student = new ShowStudent(matric_number, oFileName, this);

        workSpace->addSubWindow(student);
        student->show();
    }
}

void MainWindow::performAdminTasks()
{
    AdminForm *adminForm = new AdminForm(workSpace, this);
    workSpace->addSubWindow(adminForm);
    adminForm->setWindowModality(Qt::WindowModality::ApplicationModal);
    adminForm->show();
}
