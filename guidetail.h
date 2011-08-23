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

#ifndef DETAILWINDOW_H
#define DETAILWINDOW_H

#include <QtCore>
#include <QMainWindow>
#include <QThread>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QLabel>

#include <vector>

#include "preferences.h"
#include "keyfinderworkerthread.h"
#include "chromagram.h"
#include "metadatataglib.h"
#include "guivisuals.h"


namespace Ui {
	class DetailWindow;
}

class DetailWindow : public QMainWindow{
	Q_OBJECT
public:
	explicit DetailWindow(QWidget *parent = 0, QString path = "");
	~DetailWindow();
private:
	Preferences prefs;
	QString filePath;
	KeyFinderWorkerThread* modelThread;
	bool allowDrops;
	void dragEnterEvent(QDragEnterEvent*);
	void dropEvent(QDropEvent*);
	void processCurrentFile();
	void cleanUpAfterRun();
	// UI
	Ui::DetailWindow* ui;
	Visuals* vis;
	std::vector<QLabel*> keyLabels;
	QImage chromagramImage;
	QImage miniChromagramImage;
	QImage harmonicChangeImage;
	QImage colourScaleImage;
	int chromaScaleV;
	int chromaScaleH;
	QImage imageFromChromagram(const Chromagram*);
private slots:
	// interaction with model thread
	void criticalError(const QString&);
	void decoded();
	void madeMono();
	void downsampled();
	void receiveFullChromagram(const Chromagram&);
	void receiveOneOctaveChromagram(const Chromagram&);
	void receiveHarmonicChangeSignal(const std::vector<double>&);
	void receiveKeyEstimates(const std::vector<int>&);
	void receiveGlobalKeyEstimate(int);
	// UI
	void say(const QString&);
	void on_chromaColourCombo_currentIndexChanged(int index);
	void on_runButton_clicked();
	void layoutScaling();
	void blankVisualisations();
	void deleteKeyLabels();
	void blankKeyLabel();
	void drawPianoKeys();
	void drawColourScale();
};

#endif
