#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>

struct I: public QThread
{
    static void sleep(unsigned long secs)
    {
        QThread::sleep(secs);
    }
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MainWindow mainwindow;
    QPixmap pixmap(":/images/logo.png");
    QSplashScreen splash(&mainwindow, pixmap, Qt::WindowStaysOnTopHint);

    splash.showMaximized();
    splash.showMessage("Wait...", Qt::AlignLeft | Qt::AlignBottom, Qt::black);
    qApp->processEvents();

    I::sleep(5);
    mainwindow.show();
    splash.finish(&mainwindow);

    return app.exec();
}
