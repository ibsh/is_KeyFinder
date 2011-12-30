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
//#include <QtXml>
//#include <Qt/QXmlQuery.h> // is this the right path? Can't find symbols.

#include <vector>

#include "guidetail.h"
#include "preferences.h"
#include "guivisuals.h"
#include "keyfinderworkerthread.h"
#include "metadatataglib.h"
#include "guimenuhandler.h"

namespace Ui {
	class BatchWindow;
}

class BatchWindow : public QMainWindow{
	Q_OBJECT
public:
  explicit BatchWindow(MainMenuHandler* handler, QWidget* parent = 0);
	~BatchWindow();
private:
	// analysis
	Preferences prefs;
  vector<KeyFinderWorkerThread*> modelThreads;
	// batch processing
	bool allowDrops;
	void dragEnterEvent(QDragEnterEvent*);
	void dropEvent(QDropEvent*);
	QFutureWatcher<void> fileDropWatcher;
	void filesDropped(QList<QUrl>&);
	QStringList getDirectoryContents(QDir);
  void loadPlaylistM3u(QString);
  void loadPlaylistXml(QString);
	void addNewRow(QString);
	void getMetadata();
  bool writeToTagsAtRow(int);
  int nextFile;
  void processFiles();
  bool cancel;
	void cleanUpAfterRun();
	// UI
	Ui::BatchWindow* ui;
	Visuals* vis;
	QLabel* initialHelpLabel;
  MainMenuHandler* menuHandler;
private slots:
  void fileFailed(int);
  void fileFinished(int,int);
	void fileDropFinished();
	void on_runBatchButton_clicked();
	void copySelectedFromTableWidget();
	void writeDetectedToTags();
	void clearDetected();
	void runDetailedAnalysis();
  void on_cancelBatchButton_clicked();
};

#endif
