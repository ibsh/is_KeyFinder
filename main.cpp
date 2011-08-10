/*************************************************************************

	Copyright 2011 Ibrahim Sha'ath

	This file is part of KeyFinder.

	KeyFinder is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	KeyFinder is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with KeyFinder.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

/*

Dependencies:
	libav (0.7)
	fftw (3.2.2)
	libsndfile (1.0.24) // not strictly necessary, since libav can do the job just as well (though not as quietly)
	libsamplerate (0.1.7) // would be nicer to adapt the filter design code, since it's faster and apparently better.
	taglib (1.7, with some custom changes)

Configure commands used to build them:
	FFTW3 ./configure CC="gcc -arch i386 -arch x86_64" CXX="g++ -arch i386 -arch x86_64" CPP="gcc -E" CXXCPP="g++ -E"
	LIBSAMPLERATE same as fftw
	LIBSNDFILE ./configure CC="gcc -arch x86_64" CXX="g++ -arch x86_64" CPP="gcc -E" CXXCPP="g++ -E"
	LIBAV ./configure --enable-shared --disable-network --disable-protocol=udp --disable-protocol=tcp --disable-protocol=rtp --arch=x86_64
	LIBAV ./configure --arch=x86_64 --extra-cflags='-arch x86_64' --target-os=darwin --extra-ldflags='-arch x86_64' --enable-cross-compile --disable-ffmpeg --disable-ffplay --disable-ffserver --disable-doc --enable-shared --disable-network --disable-protocol=udp --disable-protocol=tcp --disable-protocol=rtp
	TAGLIB cmake -DWITH_MP4=ON -DWITH_ASF=ON -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_RELEASE_TYPE=Release .

*/

#include <QtGui/QApplication>
#include <QMenuBar>
#include <QKeySequence>
#include "detailwindow.h"
#include "batchwindow.h"
#include "mainmenuhandler.h"

int main(int argc, char *argv[]){
	QCoreApplication::setOrganizationName("Ibrahim Sha'ath");
	QCoreApplication::setOrganizationDomain("ibrahimshaath.com");
	QCoreApplication::setApplicationName("KeyFinder");

	QApplication a(argc, argv);

	QMenuBar* menuBar = new QMenuBar(0);
	MainMenuHandler* menuHandler = new MainMenuHandler(0);

	QMenu* menuFile = menuBar->addMenu("File");
	QAction* actionNew_Detail_Keyfinder = menuFile->addAction("New Detail Window",menuHandler,SLOT(new_Detail_Window()),QKeySequence("Ctrl+N"));
	QAction* actionNew_Batch_Keyfinder = menuFile->addAction("New Batch Window",menuHandler,SLOT(new_Batch_Window()),QKeySequence("Ctrl+Shift+N"));
	menuFile->addSeparator();
	QAction* actionClose_Window = menuFile->addAction("Close Window",menuHandler,SLOT(close_Window()),QKeySequence("Ctrl+W"));
	// These actions won't be appended to the File menu, Qt intercepts them and puts them in the Mac-native places
	QAction* actionAbout = menuFile->addAction("About",menuHandler,SLOT(about()));
	QAction* actionPreferences = menuFile->addAction("Preferences",menuHandler,SLOT(preferences()));

	BatchWindow w;
	w.show();
	return a.exec();
}
