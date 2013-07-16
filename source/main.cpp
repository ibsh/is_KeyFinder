/*************************************************************************

  Copyright 2011-2013 Ibrahim Sha'ath

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

#include <QApplication>
#include <QMenuBar>
#include <QKeySequence>

#include "guidetail.h"
#include "guibatch.h"
#include "guimenuhandler.h"
#include "decoderlibav.h"
#include "asynckeyresult.h"

#include <fstream>

void LoggingHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  std::ofstream logfile;
#if defined Q_OS_MAC
  logfile.open(QDir::homePath().toUtf8() + "/Library/Logs/KeyFinder.log", std::ios::app);
#elif defined Q_OS_LINUX
  logfile.open("KeyFinder.log", std::ios::app);
#else
  logfile.open("KeyFinder_log.txt", std::ios::app);
#endif
  logfile << QDate::currentDate().toString("yyyy-MM-dd").toUtf8().constData() << " ";
  logfile << QTime::currentTime().toString("hh:mm:ss.zzz").toUtf8().constData() << " ";
  switch (type) {
  case QtDebugMsg:
    logfile << "Debug";    break;
  case QtWarningMsg:
    logfile << "Warning";  break;
  case QtCriticalMsg:
    logfile << "Critical"; break;
  case QtFatalMsg:
    logfile << "Fatal";    break;
  }
  logfile << ": " << msg.toUtf8().constData() << " (" << context.file << ":" << context.line << ")\n";
  logfile.close();
  if (type == QtFatalMsg) {
    abort();
  }
}

int commandLineInterface(int argc, char* argv[]) {

  QString filePath = "";
  bool writeToTags = false;

  for (int i = 1; i < argc; i++) {
    if (std::strcmp(argv[i], "-f") == 0 && i+1 < argc)
      filePath = argv[++i];
    else if (std::strcmp(argv[i], "-w") == 0)
      writeToTags = true;
  }
  if (filePath.isEmpty())
    return -1; // not a valid CLI attempt, launch GUI

  Preferences prefs;
  AsyncFileObject object(filePath, prefs, 0);
  KeyFinderResultWrapper result = keyDetectionProcess(object);
  if (!result.errorMessage.isEmpty()) {
    std::cerr << result.errorMessage.toUtf8().constData();
    return 1;
  }

  std::cout << prefs.getKeyCode(result.core.globalKeyEstimate).toUtf8().constData();

  if (writeToTags) {
    TagLibMetadata md(filePath);
    MetadataWriteResult written = md.writeKeyToMetadata(result.core.globalKeyEstimate,prefs);
    bool found = false;
    for (int i = 0; i < written.newTags.size(); i++)
      if (!written.newTags[i].isEmpty()) found = true;
    if (!found) {
      std::cerr << "Could not write to tags" << std::endl;
      return 2;
    }
  }

  return 0;

}

int main(int argc, char* argv[]) {

  QCoreApplication::setOrganizationName("Ibrahim Sha'ath");
  QCoreApplication::setOrganizationDomain("ibrahimshaath.co.uk");
  QCoreApplication::setApplicationName(GuiStrings::getInstance()->appName());

  // libav setup
  av_register_all();
  av_log_set_level(AV_LOG_ERROR);
  av_lockmgr_register(NULL);

  // primitive command line use
  if (argc > 2) {
    int cliResult = commandLineInterface(argc,argv);
    if (cliResult >= 0)
      return cliResult;
  }

  qInstallMessageHandler(LoggingHandler);

  QApplication a(argc, argv);

  QString localeId = QLocale::system().name();
  QString languageId = QLocale::system().uiLanguages().first();
  localeId.replace(QRegExp("[a-z]+_"), QString(languageId +"_"));
  QLocale myMacLocale(localeId);

  QString appTranslationPathBase = "%1/Translations/is_keyfinder_%2.qm";
#if defined Q_OS_MAC
  QDir dir(QApplication::applicationDirPath());
  dir.cdUp();
  QString appTranslationPath = appTranslationPathBase.arg(dir.absolutePath()).arg(myMacLocale.name());
  QString qtTranslationName = "qt_" + myMacLocale.name();
#else
  QString appTranslationPath = localeParam.arg(QCoreApplication::applicationDirPath()).arg(QLocale::system().name());
  QString qtTranslationName = "qt_" + QLocale::system().name();
#endif

  QTranslator qtTranslator;
  qtTranslator.load(qtTranslationName, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  a.installTranslator(&qtTranslator);

  QTranslator myappTranslator;
  myappTranslator.load(appTranslationPath);
  a.installTranslator(&myappTranslator);

  MainMenuHandler* menuHandler = new MainMenuHandler(0);
  menuHandler->new_Batch_Window(true);

  return a.exec();
}
