#include <QtGui/QApplication>
#include "detailwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DetailWindow w;
    w.show();

    return a.exec();
}
