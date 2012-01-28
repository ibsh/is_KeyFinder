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

#include <QtGui/QApplication>
#include <QMenuBar>
#include <QKeySequence>
#include "guidetail.h"
#include "guibatch.h"
#include "guimenuhandler.h"

#include <fstream>

#ifndef INT64_C
#define UINT64_C(c) (c ## ULL)
#endif
#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096
extern "C"{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
}

void MacLoggingHandler(QtMsgType type, const char *msg) {
  std::ofstream logfile;
	logfile.open(QDir::homePath().toLocal8Bit() + "/Library/Logs/KeyFinder.log",std::ios::app);
	logfile << QDate::currentDate().toString("yyyy-MM-dd").toLocal8Bit().data() << " ";
	logfile << QTime::currentTime().toString("hh:mm:ss.zzz").toLocal8Bit().data() << " ";
  switch (type) {
  case QtDebugMsg:
    logfile << "Debug: " << msg << "\n";
    break;
  case QtCriticalMsg:
    logfile << "Critical: " << msg << "\n";
    break;
  case QtWarningMsg:
    logfile << "Warning: " << msg << "\n";
    break;
  case QtFatalMsg:
    logfile << "Fatal: " << msg << "\n";
    abort();
  }
  logfile.close();
}

#include <iostream>
int main(int argc, char *argv[]){

  av_register_all(); // first things first.

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
