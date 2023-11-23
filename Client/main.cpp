#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow *w = new MainWindow();

    if (w->createClient())
        w->show();
    else
        return 0;

    return a.exec();
}
