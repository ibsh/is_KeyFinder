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

#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include <QDialog>
#include <QString>
#include <QFileDialog>

#include "preferences.h"

namespace Ui {
  class PrefsDialog;
}

class PrefsDialog : public QDialog{
  Q_OBJECT
public:
  explicit PrefsDialog(QWidget *parent = 0);
  ~PrefsDialog();
private:
  // map combobox indices to useful values
  QList<tag_format_t>                    listTagFormat;
  QList<float>                           listStartingFreq;
  QList<unsigned int>                    listHopSize;
  QList<KeyFinder::temporal_window_t>    listTemporalWindow;
  QList<unsigned int>                    listFftFrameSize;
  QList<KeyFinder::tuning_method_t>      listTuningMethod;
  QList<KeyFinder::segmentation_t>       listSegmentation;
  QList<KeyFinder::tone_profile_t>       listToneProfile;
  QList<KeyFinder::similarity_measure_t> listSimilarityMeasure;
  // altering state on field changes
  void tuningEnabled();
  void binAdaptiveTuningEnabled();
  void segmentationEnabled();
  void customProfileEnabled();
  void filenameDelimiterEnabled();
private slots:
  void on_savePrefsButton_clicked();
  void on_cancelButton_clicked();
  void on_bps_valueChanged(int arg1);
  void on_segmentation_currentIndexChanged(int);
  void on_toneProfile_currentIndexChanged(int);
  void on_tuningMethod_currentIndexChanged(int);
  void on_writeToFilePrefix_stateChanged(int arg1);
  void on_writeToFileSuffix_stateChanged(int arg1);
  void on_findITunesLibraryButton_clicked();
  void on_findTraktorLibraryButton_clicked();
  void on_findSeratoLibraryButton_clicked();

private:
  Ui::PrefsDialog *ui;
};

#endif // PREFSDIALOG_H
