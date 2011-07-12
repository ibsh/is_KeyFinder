/*

Dependencies:
	libav (0.7)
	fftw (3.2.2)
	libsndfile (1.0.24) // not strictly necessary, since libav can do the job just as well (though not as quietly)
	libsamplerate (0.1.7)
	taglib?

Configure commands used to build them:
	FFTW3 ./configure CC="gcc -arch i386 -arch x86_64" CXX="g++ -arch i386 -arch x86_64" CPP="gcc -E" CXXCPP="g++ -E"
	LIBSAMPLERATE same as fftw
	LIBSNDFILE ./configure CC="gcc -arch x86_64" CXX="g++ -arch x86_64" CPP="gcc -E" CXXCPP="g++ -E"
	LIBAV ./configure --enable-shared --disable-network --disable-protocol=udp --disable-protocol=tcp --disable-protocol=rtp --arch=x86_64
	LIBAV ./configure --arch=x86_64 --extra-cflags='-arch x86_64' --target-os=darwin --extra-ldflags='-arch x86_64' --enable-cross-compile --disable-ffmpeg --disable-ffplay --disable-ffserver --disable-doc --enable-shared --disable-network --disable-protocol=udp --disable-protocol=tcp --disable-protocol=rtp

*/

#include <QtGui/QApplication>
#include "detailwindow.h"
#include "batchwindow.h"

int main(int argc, char *argv[]){
	QCoreApplication::setOrganizationName("Ibrahim");
	QCoreApplication::setApplicationName("KeyFinder");
	QApplication a(argc, argv);
	BatchWindow w;
	// QIcon icn;
	// icn.addFile("is_KeyFinder.icns");
	// a.setWindowIcon(icn);
	w.show();
	return a.exec();
}
