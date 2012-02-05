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
#include "keyfinderworker.h"
#include "keyfinderresultset.h"
#include "chromagram.h"
#include "metadatataglib.h"

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

  QFuture<KeyFinderResultSet> analysisFuture;
  QFutureWatcher<KeyFinderResultSet> analysisWatcher;

	bool allowDrops;
	void dragEnterEvent(QDragEnterEvent*);
	void dropEvent(QDropEvent*);
  void runAnalysis();
  void cleanUpAfterRun();

	// UI
	Ui::DetailWindow* ui;
	std::vector<QLabel*> keyLabels;
	QImage chromagramImage;
	QImage miniChromagramImage;
	QImage harmonicChangeImage;
	QImage colourScaleImage;
	int chromaScaleV;
	int chromaScaleH;
  QImage imageFromChromagram(const Chromagram&);
private slots:
	// interaction with model thread
  void analysisFinished();
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
