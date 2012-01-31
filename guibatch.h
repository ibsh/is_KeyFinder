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
#include <Qt/QtXml>
#include <Qt/QtXmlPatterns>
#include <QXmlResultItems>

#include <vector>

#include "guidetail.h"
#include "preferences.h"
#include "keyfinderworker.h"
#include "keyfinderresultset.h"
#include "metadatataglib.h"
#include "guimenuhandler.h"

namespace Ui {
	class BatchWindow;
}

class BatchWindow : public QMainWindow{
	Q_OBJECT
public:
  explicit BatchWindow(MainMenuHandler* handler, QWidget* parent = 0);
  bool receiveUrls(const QList<QUrl>&);
	~BatchWindow();
private:
	// analysis
	Preferences prefs;
	// batch processing
	bool allowDrops;
	void dragEnterEvent(QDragEnterEvent*);
	void dropEvent(QDropEvent*);
  QList<QUrl> droppedFiles;
  void addDroppedFiles();
  QFutureWatcher<void> addFileWatcher;

  QList<QUrl> getDirectoryContents(QDir) const;
  QList<QUrl> loadPlaylistM3u(QString) const;
  QList<QUrl> loadPlaylistXml(QString) const;
	void addNewRow(QString);
	void getMetadata();

  QFuture<KeyFinderResultSet> analysisFuture;
  QFutureWatcher<KeyFinderResultSet> analysisWatcher;
  void runAnalysis();
  void cleanUpAfterRun();

  bool writeToTagsAtRow(int);
	// UI
	Ui::BatchWindow* ui;
	QLabel* initialHelpLabel;
  MainMenuHandler* menuHandler;
private slots:
	void fileDropFinished();
	void on_runBatchButton_clicked();
  void on_cancelBatchButton_clicked();
	void copySelectedFromTableWidget();
	void writeDetectedToTags();
	void clearDetected();
	void runDetailedAnalysis();

  void analysisFinished();
  void analysisCancelled();
  void progressRangeChanged(int, int);
  void progressValueChanged(int);
  void resultReadyAt(int);
};

#endif
