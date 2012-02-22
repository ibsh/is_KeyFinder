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

#include "guiprefs.h"
#include "ui_prefsdialog.h"

PrefsDialog::PrefsDialog(QWidget *parent): QDialog(parent),ui(new Ui::PrefsDialog){
  ui->setupUi(this);
  this->setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

  // these strings store the chars relating to each index of their dropdowns
  temporalWindowComboIndex = "bmn";
  hcdfComboIndex = "nhca";
  similarityMeasureComboIndex = "ck";
  tagFormatComboIndex = "kcb";

  // get values from preferences
  Preferences p;
  ui->writeTagsAutomatically->setChecked(p.getWriteTagsAutomatically());
  ui->parallelBatchJobs->setChecked(p.getParallelBatchJobs());
  ui->writeToTagComment->setChecked(p.getWriteToTagComment());
  ui->writeToTagGrouping->setChecked(p.getWriteToTagGrouping());
  ui->writeToTagKey->setChecked(p.getWriteToTagKey());
  ui->skipFilesWithExistingTags->setChecked(p.getSkipFilesWithExistingTags());
  ui->readITunesLibrary->setChecked(p.getReadITunesLibrary());
  ui->temporalWindow->setCurrentIndex(temporalWindowComboIndex.indexOf(p.getTemporalWindow()));
  ui->hcdf->setCurrentIndex(hcdfComboIndex.indexOf(p.getHcdf()));
  ui->similarityMeasure->setCurrentIndex(similarityMeasureComboIndex.indexOf(p.getSimilarityMeasure()));
  ui->tagFormat->setCurrentIndex(tagFormatComboIndex.indexOf(p.getTagFormat()));
  ui->hopSize->setCurrentIndex(log2(p.getHopSize()/128));
  ui->fftFrameSize->setCurrentIndex(log2(p.getFftFrameSize()/512));
  ui->octaves->setValue(p.getOctaves());
  ui->bps->setValue(p.getBpo()/12);
  ui->octaveOffset->setChecked(p.getOffsetToC());
  ui->dFactor->setValue(p.getDFactor());
  ui->toneProfile->setCurrentIndex(p.getToneProfile());
  ui->hcdfPeakPickingNeighbours->setValue(p.getHcdfPeakPickingNeighbours());
  ui->hcdfArbitrarySegments->setValue(p.getHcdfArbitrarySegments());
  ui->hcdfGaussianSize->setValue(p.getHcdfGaussianSize());
  ui->tuningMethod->setCurrentIndex(p.getTuningMethod());
  ui->hcdfGaussianSigma->setValue(p.getHcdfGaussianSigma());
  ui->stFreq->setCurrentIndex(log2(p.getStartingFreqA()/27.5));
  ui->directSkStretch->setValue(p.getDirectSkStretch());
  ui->detunedBandWeight->setValue(p.getDetunedBandWeight());
  ui->iTunesLibraryPath->setText(p.getITunesLibraryPath());

  QList<float> ctp = p.getCustomToneProfile();
  ui->maj0->setValue(ctp[0]);
  ui->maj1->setValue(ctp[1]);
  ui->maj2->setValue(ctp[2]);
  ui->maj3->setValue(ctp[3]);
  ui->maj4->setValue(ctp[4]);
  ui->maj5->setValue(ctp[5]);
  ui->maj6->setValue(ctp[6]);
  ui->maj7->setValue(ctp[7]);
  ui->maj8->setValue(ctp[8]);
  ui->maj9->setValue(ctp[9]);
  ui->maj10->setValue(ctp[10]);
  ui->maj11->setValue(ctp[11]);
  ui->min0->setValue(ctp[12]);
  ui->min1->setValue(ctp[13]);
  ui->min2->setValue(ctp[14]);
  ui->min3->setValue(ctp[15]);
  ui->min4->setValue(ctp[16]);
  ui->min5->setValue(ctp[17]);
  ui->min6->setValue(ctp[18]);
  ui->min7->setValue(ctp[19]);
  ui->min8->setValue(ctp[20]);
  ui->min9->setValue(ctp[21]);
  ui->min10->setValue(ctp[22]);
  ui->min11->setValue(ctp[23]);

  QStringList ckc = p.getCustomKeyCodes();
  ui->majKey0->setText(ckc[0]);
  ui->majKey1->setText(ckc[2]);
  ui->majKey2->setText(ckc[4]);
  ui->majKey3->setText(ckc[6]);
  ui->majKey4->setText(ckc[8]);
  ui->majKey5->setText(ckc[10]);
  ui->majKey6->setText(ckc[12]);
  ui->majKey7->setText(ckc[14]);
  ui->majKey8->setText(ckc[16]);
  ui->majKey9->setText(ckc[18]);
  ui->majKey10->setText(ckc[20]);
  ui->majKey11->setText(ckc[22]);
  ui->minKey0->setText(ckc[1]);
  ui->minKey1->setText(ckc[3]);
  ui->minKey2->setText(ckc[5]);
  ui->minKey3->setText(ckc[7]);
  ui->minKey4->setText(ckc[9]);
  ui->minKey5->setText(ckc[11]);
  ui->minKey6->setText(ckc[13]);
  ui->minKey7->setText(ckc[15]);
  ui->minKey8->setText(ckc[17]);
  ui->minKey9->setText(ckc[19]);
  ui->minKey10->setText(ckc[21]);
  ui->minKey11->setText(ckc[23]);
  ui->silence->setText(ckc[24]);

  // enable/disable fields as necessary
  tuningEnabled();
  binAdaptiveTuningEnabled();
  hcdfEnabled();
  customProfileEnabled();
  readITunesProfileEnabled();

  //relative sizing on Mac only
#ifdef Q_OS_MAC
  QFont smallerFont;
  smallerFont.setPointSize(smallerFont.pointSize() - 2);
  for(int i=0; i<ui->gridLayoutKeyCodes->count(); i++)
    if(!ui->gridLayoutKeyCodes->itemAt(i)->isEmpty())
      if(ui->gridLayoutKeyCodes->itemAt(i)->widget()->inherits("QLabel"))
        ui->gridLayoutKeyCodes->itemAt(i)->widget()->setFont(smallerFont);
  for(int i=0; i<ui->customMajor->count(); i++)
    if(!ui->customMajor->itemAt(i)->isEmpty())
      if(ui->customMajor->itemAt(i)->widget()->inherits("QLabel"))
        ui->customMajor->itemAt(i)->widget()->setFont(smallerFont);
  for(int i=0; i<ui->customMinor->count(); i++)
    if(!ui->customMinor->itemAt(i)->isEmpty())
      if(ui->customMinor->itemAt(i)->widget()->inherits("QLabel"))
        ui->customMinor->itemAt(i)->widget()->setFont(smallerFont);
#endif
}

PrefsDialog::~PrefsDialog(){
  delete ui;
}

void PrefsDialog::on_savePrefsButton_clicked(){
  Preferences p;
  p.setWriteTagsAutomatically(ui->writeTagsAutomatically->isChecked());
  p.setParallelBatchJobs(ui->parallelBatchJobs->isChecked());
  p.setWriteToTagComment(ui->writeToTagComment->isChecked());
  p.setWriteToTagGrouping(ui->writeToTagGrouping->isChecked());
  p.setWriteToTagKey(ui->writeToTagKey->isChecked());
  p.setSkipFilesWithExistingTags(ui->skipFilesWithExistingTags->isChecked());
  p.setReadITunesLibrary(ui->readITunesLibrary->isChecked());
  p.setTemporalWindow(temporalWindowComboIndex[ui->temporalWindow->currentIndex()].toAscii());
  p.setHcdf(hcdfComboIndex[ui->hcdf->currentIndex()].toAscii());
  p.setSimilarityMeasure(similarityMeasureComboIndex[ui->similarityMeasure->currentIndex()].toAscii());
  p.setTagFormat(tagFormatComboIndex[ui->tagFormat->currentIndex()].toAscii());
  p.setHopSize(pow(2,ui->hopSize->currentIndex())*128);
  p.setFftFrameSize(pow(2,ui->fftFrameSize->currentIndex())*512);
  p.setOctaves(ui->octaves->value());
  p.setBps(ui->bps->value());
  p.setOffsetToC(ui->octaveOffset->isChecked());
  p.setDFactor(ui->dFactor->value());
  p.setToneProfile(ui->toneProfile->currentIndex());
  p.setHcdfPeakPickingNeighbours(ui->hcdfPeakPickingNeighbours->value());
  p.setHcdfArbitrarySegments(ui->hcdfArbitrarySegments->value());
  p.setHcdfGaussianSize(ui->hcdfGaussianSize->value());
  p.setTuningMethod(ui->tuningMethod->currentIndex());
  p.setHcdfGaussianSigma(ui->hcdfGaussianSigma->value());
  p.setStartingFreqA(pow(2,ui->stFreq->currentIndex())*27.5);
  p.setDirectSkStretch(ui->directSkStretch->value());
  p.setDetunedBandWeight(ui->detunedBandWeight->value());
  p.setITunesLibraryPath(ui->iTunesLibraryPath->text());

  QList<float> ctp;
  ctp << ui->maj0->value() << ui->maj1->value() << ui->maj2->value() << ui->maj3->value();
  ctp << ui->maj4->value() << ui->maj5->value() << ui->maj6->value() << ui->maj7->value();
  ctp << ui->maj8->value() << ui->maj9->value() << ui->maj10->value() << ui->maj11->value();
  ctp << ui->min0->value() << ui->min1->value() << ui->min2->value() << ui->min3->value();
  ctp << ui->min4->value() << ui->min5->value() << ui->min6->value() << ui->min7->value();
  ctp << ui->min8->value() << ui->min9->value() << ui->min10->value() << ui->min11->value();
  p.setCustomToneProfile(ctp);

  QStringList ckc;
  ckc << ui->majKey0->text() << ui->majKey1->text() << ui->majKey2->text() << ui->majKey3->text();
  ckc << ui->majKey4->text() << ui->majKey5->text() << ui->majKey6->text() << ui->majKey7->text();
  ckc << ui->majKey8->text() << ui->majKey9->text() << ui->majKey10->text() << ui->majKey11->text();
  ckc << ui->minKey0->text() << ui->minKey1->text() << ui->minKey2->text() << ui->minKey3->text();
  ckc << ui->minKey4->text() << ui->minKey5->text() << ui->minKey6->text() << ui->minKey7->text();
  ckc << ui->minKey8->text() << ui->minKey9->text() << ui->minKey10->text() << ui->minKey11->text();
  ckc << ui->silence->text();
  p.setCustomKeyCodes(ckc);

  // save to QSettings and close window
  p.save();
  this->close();
}

void PrefsDialog::on_cancelButton_clicked(){
  this->close();
}

void PrefsDialog::tuningEnabled(){
  ui->tuningMethod->setEnabled(ui->bps->value() > 1);
  binAdaptiveTuningEnabled();
}

void PrefsDialog::binAdaptiveTuningEnabled(){
  ui->detunedBandWeight->setEnabled(ui->tuningMethod->isEnabled() && ui->tuningMethod->currentIndex() == 1);
}

void PrefsDialog::hcdfEnabled(){
  int chk = ui->hcdf->currentIndex();
  bool e = (chk != 0 && chk != 3);
  ui->hcdfGaussianSigma->setEnabled(e);
  ui->hcdfGaussianSize->setEnabled(e);
  ui->hcdfPeakPickingNeighbours->setEnabled(e);
  e = (chk == 3);
  ui->hcdfArbitrarySegments->setEnabled(e);
}

void PrefsDialog::customProfileEnabled(){
  bool e = (ui->toneProfile->currentText() == "Custom");
  ui->maj0->setEnabled(e);
  ui->maj1->setEnabled(e);
  ui->maj2->setEnabled(e);
  ui->maj3->setEnabled(e);
  ui->maj4->setEnabled(e);
  ui->maj5->setEnabled(e);
  ui->maj6->setEnabled(e);
  ui->maj7->setEnabled(e);
  ui->maj8->setEnabled(e);
  ui->maj9->setEnabled(e);
  ui->maj10->setEnabled(e);
  ui->maj11->setEnabled(e);
  ui->min0->setEnabled(e);
  ui->min1->setEnabled(e);
  ui->min2->setEnabled(e);
  ui->min3->setEnabled(e);
  ui->min4->setEnabled(e);
  ui->min5->setEnabled(e);
  ui->min6->setEnabled(e);
  ui->min7->setEnabled(e);
  ui->min8->setEnabled(e);
  ui->min9->setEnabled(e);
  ui->min10->setEnabled(e);
  ui->min11->setEnabled(e);
}

void PrefsDialog::readITunesProfileEnabled(){
  ui->findITunesLibraryButton->setEnabled(ui->readITunesLibrary->isChecked());
}

void PrefsDialog::on_bps_valueChanged(int /*arg1*/){
  tuningEnabled();
}

void PrefsDialog::on_tuningMethod_currentIndexChanged(int /*index*/){
  binAdaptiveTuningEnabled();
}

void PrefsDialog::on_hcdf_currentIndexChanged(int /*index*/){
  hcdfEnabled();
}

void PrefsDialog::on_toneProfile_currentIndexChanged(int /*index*/){
  customProfileEnabled();
}

void PrefsDialog::on_readITunesLibrary_toggled(bool /*checked*/){
  readITunesProfileEnabled();
}

void PrefsDialog::on_findITunesLibraryButton_clicked(){
  QString initDir;
#ifdef Q_OS_WIN
  initDir = QDir::homePath() + "/My Music";
#else
  initDir = QDir::homePath() + "/Music/iTunes";
#endif
  QString iTunesPath = QFileDialog::getOpenFileName(this,tr("Choose iTunes Library XML"), initDir, tr("iTunes XML (*.xml)"));
  if(iTunesPath.isEmpty())
    return;
  ui->iTunesLibraryPath->setText(iTunesPath);
}
