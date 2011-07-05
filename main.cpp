#include <QtGui/QApplication>
#include "detailwindow.h"
#include "batchwindow.h"

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
		BatchWindow w;
		QIcon icn;
		icn.addFile("is_KeyFinder.icns");
		a.setWindowIcon(icn);
		w.show();
    return a.exec();
}
