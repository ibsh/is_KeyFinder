#ifndef DETAILWINDOW_H
#define DETAILWINDOW_H

#include <QtCore>
#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QFuture>
#include <QFutureWatcher>
#include <QLabel>

#include <string>
#include <vector>

#include "batchwindow.h" // need to be able to launch other window

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
	class DetailWindow;
}

class DetailWindow : public QMainWindow{
	Q_OBJECT
public:
	explicit DetailWindow(QWidget *parent = 0);
	~DetailWindow();
private:
	// UI
	Ui::DetailWindow* ui;
	Visuals* vis;
	void drawPianoKeys();
	QImage imageFromChromagram(Chromagram*);
	std::vector<QLabel*> keyLabels;
	QImage chromagramImage;
	QImage miniChromagramImage;
	QImage harmonicChangeImage;
	QImage colourScaleImage;
	int chromaScaleV;
	int chromaScaleH;
	// analysis
	Preferences prefs;
	AudioBuffer* ab;
	SpectrumAnalyser* sa;
	Chromagram* ch;
	std::vector<double> rateOfChange;
	std::vector<int> keys;
	//processing files
	std::string filePath;
	bool allowDrops;
	void dragEnterEvent(QDragEnterEvent*);
	void dropEvent(QDropEvent*);
	void go();															// begin steps
	void decode();													// step 1
	QFutureWatcher<void> decodeWatcher;
	void makeMono();												// step 2
	QFutureWatcher<void> monoWatcher;
	void downSample();											// step 3
	QFutureWatcher<void> dsWatcher;
	void spectrumAnalysis();								// step 4
	QFutureWatcher<void> saWatcher;
	void harmonicAnalysis();								// step 5
	QFutureWatcher<void> haWatcher;
	void cleanUpAfterRun();
private slots:
	void decoded();													// step 1 complete
	void madeMono();												// step 2 complete
	void downSampled();											// step 3 complete
	void spectrumAnalysisComplete();				// step 4 complete
	void haFinished();											// step 5 complete
	void say(const QString&);
	void on_actionNew_Detail_Keyfinder_triggered();
	void on_actionNew_Batch_Keyfinder_triggered();
	void on_actionClose_Window_triggered();
	void on_spectrumAnalyserCombo_currentIndexChanged(int index);
	void on_temporalWindowCombo_currentIndexChanged(int index);
	void on_toneProfilesCombo_currentIndexChanged(int index);
	void on_chromaColourCombo_currentIndexChanged(int index);
	void on_runButton_clicked();
public slots:
	void analyse(std::string);
};

#endif
