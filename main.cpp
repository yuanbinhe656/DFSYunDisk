#include "mainwindow.h"
#include <login.h>
#include <QApplication>
#include <mytitlebar.h>

#
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    Login w;
//    MyTitleBar w;
    //Title w;
    //Form w;
    w.show();
    return a.exec();
}
