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

#include <QtCore>
#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QFuture>
#include <QFutureWatcher>
#include <QClipboard>
#include <QMessageBox>
#include <QLabel>

#include <string>
#include <vector>

#include "detailwindow.h"
#include "aboutdialog.h"

#include "preferences.h"
#include "visuals.h"
#include "exception.h"
#include "audiobuffer.h"
#include "chromagram.h"

#include "decoder.h"
#include "monaural.h"
#include "downsampler.h"
#include "spectrumanalyserfactory.h"
#include "hcdf.h"
#include "keyclassifier.h"

#include "metadata.h"

namespace Ui {
	class BatchWindow;
}

class BatchWindow : public QMainWindow{
	Q_OBJECT
public:
	explicit BatchWindow(QWidget *parent = 0);
	~BatchWindow();
private:
	// UI
	Ui::BatchWindow* ui;
	Visuals* vis;
	QByteArray copyArray;
	QLabel* initialHelp;
	// analysis
	Preferences prefs;
	//processing files
	void dragEnterEvent(QDragEnterEvent*);
	void dropEvent(QDropEvent*);
	int currentFile;
	bool allowDrops;
	void processCurrentFile();
	void cleanUpAfterRun();
	void analyseFile(int);
	void markBroken(int);
	void filesDropped(QList<QUrl>&);
	void addNewRow(QString);
	QStringList getDirectoryContents(QDir);
	QFutureWatcher<void> analysisWatcher;
	QFutureWatcher<void> fileDropWatcher;
private slots:
	void fileFinished();
	void fileDropFinished();
	void on_runBatchButton_clicked();
	void copySelectedFromTableWidget();
	void writeDetectedToGrouping();
	void runDetailedAnalysis();
};

#endif
