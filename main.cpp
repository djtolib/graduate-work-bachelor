#include "mainwindow.h"
#include <QApplication>
#include <QFrame>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.setApplicationName("imView");
    return a.exec();
}
