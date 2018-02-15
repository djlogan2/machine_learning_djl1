#include <cstdlib>         // for srand, NULL
#include "mainwindow.h"    // for MainWindow
#include <QApplication>    // for QApplication

int main(int argc, char *argv[])
{
    std::srand(std::time(NULL));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
