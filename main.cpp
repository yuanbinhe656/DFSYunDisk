#include "mainwindow.h"
#include <login.h>
#include <QApplication>
#include "buttongroup.h"
#include <myfilewg.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //MyFileWg w;
   //  ButtonGroup w;
    Login w;
//    MyTitleBar w;
    //Title w;
    //Form w;

    w.show();
    return a.exec();
}
