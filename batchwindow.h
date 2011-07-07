#ifndef BATCHWINDOW_H
#define BATCHWINDOW_H

#include <QtCore>
#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QClipboard>
#include <QFuture>
#include <QFutureWatcher>

#include <string>
#include <vector>

#include "detailwindow.h"

#include "preferences.h"
#include "visuals.h"
#include "fatalexception.h"
#include "audiobuffer.h"
#include "chromagram.h"
#include "windowfunctions.h"

#include "decoder.h"
#include "decoderlibav.h"
#include "decoderlibsndfile.h"

#include "downsampler.h"
#include "downsamplerlibsrc.h"
#include "downsamplerib.h"

#include "monaural.h"
#include "basicmono.h"

#include "spectrumanalyserfactory.h"

#include "hcdfharte.h"
#include "harmonicclassifier.h"

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
	// analysis
	Preferences prefs;
	AudioBuffer* ab;
	SpectrumAnalyser* sa;
	Chromagram* ch;
	//processing files
	void dragEnterEvent(QDragEnterEvent*);
	void dropEvent(QDropEvent*);
	int currentFile;
	void go();
	void cleanUpAfterRun();
	void analyseFile(int);
	void markBroken(int);
	void filesDropped(QList<QUrl>);
	void keyPressEvent(QKeyEvent*);
	QFutureWatcher<void> analysisWatcher;
	QFutureWatcher<void> fileDropWatcher;
private slots:
	void fileFinished();
	void fileDropFinished();
	void on_actionNew_Detail_Keyfinder_triggered();
	void on_actionNew_Batch_Keyfinder_triggered();
	void on_actionClose_Window_triggered();
	void on_runBatchButton_clicked();
	void on_tableWidget_itemSelectionChanged();
};

#endif
