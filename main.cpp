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
	libsamplerate (0.1.7)
	taglib (1.7)

Configure commands used to build them:
	FFTW3 ./configure CC="gcc -arch i386 -arch x86_64" CXX="g++ -arch i386 -arch x86_64" CPP="gcc -E" CXXCPP="g++ -E"
	LIBSAMPLERATE same as fftw
	LIBAV ./configure --arch=x86_64 --extra-cflags='-arch x86_64' --target-os=darwin --extra-ldflags='-arch x86_64' --enable-cross-compile --disable-ffmpeg --disable-ffplay --disable-ffserver --disable-doc --enable-shared --disable-network --disable-protocol=udp --disable-protocol=tcp --disable-protocol=rtp
	TAGLIB cmake -DWITH_MP4=ON -DWITH_ASF=ON -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_RELEASE_TYPE=Release .

*/

#include <QtGui/QApplication>
#include <QMenuBar>
#include <QKeySequence>
#include "guidetail.h"
#include "guibatch.h"
#include "guimenuhandler.h"

#include <fstream>

void SimpleLoggingHandler(QtMsgType type, const char *msg) {
	std::ofstream logfile;
	logfile.open(QDir::homePath().toAscii() + "/Library/Logs/KeyFinder.log",std::ios::app);
	switch (type) {
	case QtDebugMsg:
		logfile << QTime::currentTime().toString().toAscii().data() << " Debug: " << msg << "\n";
		break;
	case QtCriticalMsg:
		logfile << QTime::currentTime().toString().toAscii().data() << " Critical: " << msg << "\n";
		break;
	case QtWarningMsg:
		logfile << QTime::currentTime().toString().toAscii().data() << " Warning: " << msg << "\n";
		break;
	case QtFatalMsg:
		logfile << QTime::currentTime().toString().toAscii().data() <<  " Fatal: " << msg << "\n";
		abort();
	}
	logfile.close();
}

int main(int argc, char *argv[]){
	QCoreApplication::setOrganizationName("Ibrahim Sha'ath");
	QCoreApplication::setOrganizationDomain("ibrahimshaath.com");
	QCoreApplication::setApplicationName("KeyFinder");

	QApplication a(argc, argv);

	qInstallMsgHandler(SimpleLoggingHandler);

	QMenuBar* menuBar = new QMenuBar(0);
	MainMenuHandler* menuHandler = new MainMenuHandler(0);

	QMenu* menuFile = menuBar->addMenu("File");
	menuFile->addAction("New Detail Window",menuHandler,SLOT(new_Detail_Window()),QKeySequence("Ctrl+N"));
	menuFile->addAction("New Batch Window",menuHandler,SLOT(new_Batch_Window()),QKeySequence("Ctrl+Shift+N"));
	menuFile->addSeparator();
	menuFile->addAction("Close Window",menuHandler,SLOT(close_Window()),QKeySequence("Ctrl+W"));
	// These actions won't be appended to the File menu, Qt intercepts them and puts them in the Mac-native places
	menuFile->addAction("About",menuHandler,SLOT(about()));
	menuFile->addAction("Preferences",menuHandler,SLOT(preferences()));

	BatchWindow w;
	w.show();
	return a.exec();
}
