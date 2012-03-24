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

#ifndef BATCHWINDOW_H
#define BATCHWINDOW_H

// forward declaration for circular dependency
class MainMenuHandler;

#include <QtCore>
#include <QMainWindow>
#include <QThread>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QFuture>
#include <QFutureWatcher>
#include <QClipboard>
#include <QMessageBox>
#include <QLabel>
#ifdef Q_OS_WIN
#include <QtXml/QtXml>
#include <QtXmlPatterns/QtXmlPatterns>
#else
#include <Qt/QtXml>
#include <Qt/QtXmlPatterns>
#endif
#include <QXmlResultItems>
#include <QtNetwork/QNetworkReply>

#include "guidetail.h"
#include "guimenuhandler.h"
#include "preferences.h"
#include "asynckeyprocess.h"
#include "asyncmetadatareadprocess.h"
#include "metadatataglib.h"
#include "externalplaylist.h"
#include "_VERSION.h"


namespace Ui {
  class BatchWindow;
}

class BatchWindow : public QMainWindow{
  Q_OBJECT
public:
  explicit BatchWindow(MainMenuHandler* handler, QWidget* parent = 0);
  bool receiveUrls(const QList<QUrl>&);
  ~BatchWindow();
public slots:
  void checkForNewVersion();
private:
  void closeEvent(QCloseEvent*);
  Preferences prefs;
  void setGuiDefaults();

  int libraryOldIndex;
  QFutureWatcher<QList<ExternalPlaylistObject> > readLibraryWatcher;
  QFutureWatcher<QList<QUrl> > loadPlaylistWatcher;

  void dragEnterEvent(QDragEnterEvent*);
  void dropEvent(QDropEvent*);
  QList<QUrl> droppedFiles;
  void addDroppedFiles();
  QFutureWatcher<void> addFilesWatcher;
  QList<QUrl> getDirectoryContents(QDir) const;

  void addNewRow(QString);
  QFutureWatcher<MetadataReadResult> metadataReadWatcher;
  void readMetadata();

  QFutureWatcher<KeyFinderResultWrapper> analysisWatcher;
  void checkRowsForSkipping();
  void markRowSkipped(int,bool);
  void runAnalysis();

  bool writeToTagsAtRow(int, int);
  bool writeToFilenameAtRow(int, int);

  // UI
  Ui::BatchWindow* ui;
  QLabel* initialHelpLabel;
  MainMenuHandler* menuHandler;
  QBrush keyFinderRow;
  QBrush keyFinderAltRow;
  QBrush textDefault;
  QBrush textSuccess;
  QBrush textError;
private slots:

  void on_libraryWidget_cellClicked(int,int);

  void readLibraryFinished();
  void loadPlaylistFinished();

  void addFilesFinished();
  void on_runBatchButton_clicked();
  void on_cancelBatchButton_clicked();
  void copySelectedFromTableWidget();
  void writeDetectedToFiles();
  void clearDetected();
  void deleteSelectedRows();
  void runDetailedAnalysis();

  void analysisFinished();
  void analysisResultReadyAt(int);

  void metadataReadFinished();
  void metadataReadResultReadyAt(int);

  void progressRangeChanged(int, int);
  void progressValueChanged(int);

  void receiveNetworkReply(QNetworkReply*);
};

#endif
