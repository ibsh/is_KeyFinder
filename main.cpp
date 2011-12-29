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

Configure commands used to build them on Mac:
  FFTW3 ./configure CC="gcc -arch i386 -arch x86_64" CXX="g++ -arch i386 -arch x86_64" CPP="gcc -E" CXXCPP="g++ -E"
  LIBSAMPLERATE same as fftw
  LIBAV ./configure --arch=x86_64 --extra-cflags='-arch x86_64' --target-os=darwin --extra-ldflags='-arch x86_64' --enable-cross-compile --disable-ffmpeg --disable-ffplay --disable-ffserver --disable-doc --enable-shared --disable-network --disable-protocol=udp --disable-protocol=tcp --disable-protocol=rtp
  TAGLIB cmake -DWITH_MP4=ON -DWITH_ASF=ON -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_RELEASE_TYPE=Release .

Attempted commands for dual-architecture versions:
  FFTW_32 ./configure CC="gcc -arch i386" CXX="g++ -arch i386" CPP="gcc -E" CXXCPP="g++ -E"
  FFTW_64 ./configure CC="gcc -arch x86_64" CXX="g++ -arch x86_64" CPP="gcc -E" CXXCPP="g++ -E"
  * MAKE FAILED * LIBAV_32 ./configure --arch=i386 --extra-cflags='-arch i386' --target-os=darwin --extra-ldflags='-arch i386' --enable-cross-compile --disable-ffmpeg --disable-ffplay --disable-ffserver --disable-doc --enable-shared --disable-network --disable-protocol=udp --disable-protocol=tcp --disable-protocol=rtp
  LIBAV_64 ./configure --arch=x86_64 --extra-cflags='-arch x86_64' --target-os=darwin --extra-ldflags='-arch x86_64' --enable-cross-compile --disable-ffmpeg --disable-ffplay --disable-ffserver --disable-doc --enable-shared --disable-network --disable-protocol=udp --disable-protocol=tcp --disable-protocol=rtp

*/

#include <QtGui/QApplication>
#include <QMenuBar>
#include <QKeySequence>
#include "guidetail.h"
#include "guibatch.h"
#include "guimenuhandler.h"

#include <fstream>

void MacLoggingHandler(QtMsgType type, const char *msg) {
  std::ofstream logfile;
  logfile.open(QDir::homePath().toUtf8() + "/Library/Logs/KeyFinder.log",std::ios::app);
  switch (type) {
  case QtDebugMsg:
    logfile << QTime::currentTime().toString().toUtf8().data() << " Debug: " << msg << "\n";
    break;
  case QtCriticalMsg:
    logfile << QTime::currentTime().toString().toUtf8().data() << " Critical: " << msg << "\n";
    break;
  case QtWarningMsg:
    logfile << QTime::currentTime().toString().toUtf8().data() << " Warning: " << msg << "\n";
    break;
  case QtFatalMsg:
    logfile << QTime::currentTime().toString().toUtf8().data() <<  " Fatal: " << msg << "\n";
    abort();
  }
  logfile.close();
}

#include <iostream>
int main(int argc, char *argv[]){
  QCoreApplication::setOrganizationName("Ibrahim Sha'ath");
  QCoreApplication::setOrganizationDomain("ibrahimshaath.co.uk");
  QCoreApplication::setApplicationName("KeyFinder");

  QApplication a(argc, argv);

  #ifdef Q_OS_MAC
    qInstallMsgHandler(MacLoggingHandler);
  #endif

  MainMenuHandler* menuHandler = new MainMenuHandler(0);

  menuHandler->new_Batch_Window();

  return a.exec();
}
