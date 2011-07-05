#ifndef DETAILWINDOW_H
#define DETAILWINDOW_H

#include <QtCore>
#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QLabel>
#include <QPainter>
#include <QFuture>
#include <QFutureWatcher>

#include <string>
#include <vector>

#include "batchwindow.h"

#include "preferences.h"
#include "fatalexception.h"
#include "audiobuffer.h"
#include "chromagram.h"
#include "windowfunctions.h"

#include "decoder.h"
#include "decoderlibav.h"
#include "decoderlibsndfile.h"

#include "downsampler.h"
#include "downsamplerlibsrc.h"
#include "downsamplerdiscard.h"
#include "downsamplerib.h"

#include "monaural.h"
#include "basicmono.h"

#include "spectrumanalyserfactory.h"
//#include "spectrumanalyser.h"
//#include "goertzelanalyser.h"
//#include "fftwanalyser.h"
//#include "fftpostprocessor.h"
//#include "logbinspostproc.h"
//#include "directskpostproc.h"
//#include "constantqpostproc.h"

#include "hcdfharte.h"
#include "harmonicclassifier.h"

namespace Ui {
	class DetailWindow;
}

class DetailWindow : public QMainWindow{
	Q_OBJECT
public:
	explicit DetailWindow(QWidget *parent = 0);
	~DetailWindow();
	void dragEnterEvent(QDragEnterEvent*);
	void dropEvent(QDropEvent*);
private:
	Ui::DetailWindow *ui;
	// messy bits
	void setChromagramColours(QImage&,int which = 0);
	void drawPianoKeys();
	std::string fileName;
	bool fileLoaded;
	std::vector<QString> keyNames;
	std::vector<QColor> keyColours;
	std::vector<QLabel*> keyLabels;
	Preferences prefs;
	AudioBuffer* ab;
	SpectrumAnalyser* sa;
	Chromagram* ch;
	std::vector<int> keys;
	QImage chromagramImage;
	QImage miniChromagramImage;
	//processing files
	void go();															// begin steps
	void cleanUpAfterRun();
	void decode();													// step 1
	QFutureWatcher<void> decodeWatcher;
	void makeMono();												// step 2
	QFutureWatcher<void> monoWatcher;
	void downSample();											// step 3
	QFutureWatcher<void> dsWatcher;
	void saInitialise();										// step 4
	QFutureWatcher<void> saInitWatcher;
	void saAnalyse();												// step 5
	QFutureWatcher<void> saWatcher;
	void harmonicAnalysis();								// step 6
	QFutureWatcher<void> haWatcher;
public slots:
	void decoded();													// step 1 complete
	void madeMono();												// step 2 complete
	void downSampled();											// step 3 complete
	void saInitialised();										// step 4 complete
	void saAnalysed();											// step 5 complete
	void haFinished();											// step 6 complete
	void say(const QString&);
private slots:
	void on_actionNew_Detail_Keyfinder_triggered();
	void on_actionClose_Window_triggered();
	void on_saCombo_currentIndexChanged(int index);
	void on_tpCombo_currentIndexChanged(int index);
	void on_runButton_clicked();
	void on_twCombo_currentIndexChanged(int index);
	void on_actionNew_Batch_Keyfinder_triggered();
	void on_chromaColourCombo_currentIndexChanged(int index);
};

#endif
