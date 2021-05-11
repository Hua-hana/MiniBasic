#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<std::string>("string");
    MainWindow w;
    w.show();
    return a.exec();
    // change a lien
}
