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

#include "detailwindow.h" // need to be able to launch other window

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
	AudioBuffer* ab;
	SpectrumAnalyser* sa;
	Chromagram* ch;
	//processing files
	void dragEnterEvent(QDragEnterEvent*);
	void dropEvent(QDropEvent*);
	int currentFile;
	bool allowDrops;
	void go();
	void cleanUpAfterRun();
	void analyseFile(int);
	void markBroken(int);
	void filesDropped(QList<QUrl>);
	QFutureWatcher<void> analysisWatcher;
	QFutureWatcher<void> fileDropWatcher;
private slots:
	void fileFinished();
	void fileDropFinished();
	void on_actionNew_Detail_Keyfinder_triggered();
	void on_actionNew_Batch_Keyfinder_triggered();
	void on_actionClose_Window_triggered();
	void on_runBatchButton_clicked();
	void copySelectedFromTableWidget();
	void writeDetectedToGrouping();
};

#endif
