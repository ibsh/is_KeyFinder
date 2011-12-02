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

	// initialise Preferences object to ensure initial QSettings is written
	Preferences prefsObj;

	// these strings store the chars relating to each index of their dropdowns
	temporalWindowComboIndex = "bmn";
	hcdfComboIndex = "nhca";
	similarityMeasureComboIndex = "ck";
  tagFieldComboIndex = "cgk";
  tagFormatComboIndex = "kcb";

	// set preferences from QSettings
	QSettings settings;

	settings.beginGroup("analysisFrequencies");
	ui->stFreq->setCurrentIndex(log2(settings.value("startingFrequencyA").toFloat()/27.5));
	ui->octaves->setValue(settings.value("numOctaves").toInt());
	ui->octaveOffset->setChecked(settings.value("octaveBeginOffset").toInt() > 0);
	ui->bps->setValue(settings.value("bandsPerSemitone").toInt());
	settings.endGroup();

	settings.beginGroup("spectralAnalysis");
	ui->temporalWindow->setCurrentIndex(temporalWindowComboIndex.indexOf(settings.value("temporalWindow").toChar()));
	ui->fftFrameSize->setCurrentIndex(log2(settings.value("fftFrameSize").toInt()/512));
	ui->hopSize->setCurrentIndex(log2(settings.value("hopSize").toInt()/128));
	ui->directSkStretch->setValue(settings.value("directSkStretch").toFloat());
	settings.endGroup();

	settings.beginGroup("downsampling");
	ui->dFactor->setValue(settings.value("dFactor").toInt());
	settings.endGroup();

	settings.beginGroup("tuning");
	ui->tuningMethod->setCurrentIndex(settings.value("tuningMethod").toInt());
	ui->detunedBandWeight->setValue(settings.value("detunedBandWeight").toFloat());
	settings.endGroup();

	settings.beginGroup("harmonicChangeDetectionFunction");
	ui->hcdf->setCurrentIndex(hcdfComboIndex.indexOf(settings.value("hcdf").toChar()));
	ui->hcdfGaussianSize->setValue(settings.value("hcdfGaussianSize").toInt());
	ui->hcdfGaussianSigma->setValue(settings.value("hcdfGaussianSigma").toFloat());
	ui->hcdfPeakPickingNeighbours->setValue(settings.value("hcdfPeakPickingNeighbours").toInt());
	ui->hcdfArbitrarySegments->setValue(settings.value("hcdfArbitrarySegments").toInt());
	settings.endGroup();

	settings.beginGroup("keyClassification");
  ui->toneProfile->setCurrentIndex(settings.value("toneProfile").toInt());
	ui->similarityMeasure->setCurrentIndex(similarityMeasureComboIndex.indexOf(settings.value("similarityMeasure").toChar()));
	settings.endGroup();

	settings.beginGroup("customToneProfile");
	ui->maj0->setValue(settings.value("maj0").toFloat());
	ui->maj1->setValue(settings.value("maj1").toFloat());
	ui->maj2->setValue(settings.value("maj2").toFloat());
	ui->maj3->setValue(settings.value("maj3").toFloat());
	ui->maj4->setValue(settings.value("maj4").toFloat());
	ui->maj5->setValue(settings.value("maj5").toFloat());
	ui->maj6->setValue(settings.value("maj6").toFloat());
	ui->maj7->setValue(settings.value("maj7").toFloat());
	ui->maj8->setValue(settings.value("maj8").toFloat());
	ui->maj9->setValue(settings.value("maj9").toFloat());
	ui->maj10->setValue(settings.value("maj10").toFloat());
	ui->maj11->setValue(settings.value("maj11").toFloat());
	ui->min0->setValue(settings.value("min0").toFloat());
	ui->min1->setValue(settings.value("min1").toFloat());
	ui->min2->setValue(settings.value("min2").toFloat());
	ui->min3->setValue(settings.value("min3").toFloat());
	ui->min4->setValue(settings.value("min4").toFloat());
	ui->min5->setValue(settings.value("min5").toFloat());
	ui->min6->setValue(settings.value("min6").toFloat());
	ui->min7->setValue(settings.value("min7").toFloat());
	ui->min8->setValue(settings.value("min8").toFloat());
	ui->min9->setValue(settings.value("min9").toFloat());
	ui->min10->setValue(settings.value("min10").toFloat());
	ui->min11->setValue(settings.value("min11").toFloat());
	settings.endGroup();

	settings.beginGroup("customKeyCodes");
	ui->majKey0->setText(settings.value("majKey0").toString());
	ui->majKey1->setText(settings.value("majKey1").toString());
	ui->majKey2->setText(settings.value("majKey2").toString());
	ui->majKey3->setText(settings.value("majKey3").toString());
	ui->majKey4->setText(settings.value("majKey4").toString());
	ui->majKey5->setText(settings.value("majKey5").toString());
	ui->majKey6->setText(settings.value("majKey6").toString());
	ui->majKey7->setText(settings.value("majKey7").toString());
	ui->majKey8->setText(settings.value("majKey8").toString());
	ui->majKey9->setText(settings.value("majKey9").toString());
	ui->majKey10->setText(settings.value("majKey10").toString());
	ui->majKey11->setText(settings.value("majKey11").toString());
	ui->minKey0->setText(settings.value("minKey0").toString());
	ui->minKey1->setText(settings.value("minKey1").toString());
	ui->minKey2->setText(settings.value("minKey2").toString());
	ui->minKey3->setText(settings.value("minKey3").toString());
	ui->minKey4->setText(settings.value("minKey4").toString());
	ui->minKey5->setText(settings.value("minKey5").toString());
	ui->minKey6->setText(settings.value("minKey6").toString());
	ui->minKey7->setText(settings.value("minKey7").toString());
	ui->minKey8->setText(settings.value("minKey8").toString());
	ui->minKey9->setText(settings.value("minKey9").toString());
	ui->minKey10->setText(settings.value("minKey10").toString());
	ui->minKey11->setText(settings.value("minKey11").toString());
	ui->silence->setText(settings.value("silence").toString());
	settings.endGroup();

  settings.beginGroup("tags");
  ui->tagFormat->setCurrentIndex(tagFormatComboIndex.indexOf(settings.value("tagFormat").toChar()));
  ui->tagField->setCurrentIndex(tagFieldComboIndex.indexOf(settings.value("tagField").toChar()));
  settings.endGroup();

	// enable/disable fields as necessary
	tuningEnabled();
	binAdaptiveTuningEnabled();
	hcdfEnabled();
	customProfileEnabled();
}

PrefsDialog::~PrefsDialog(){
	delete ui;
}

void PrefsDialog::on_savePrefsButton_clicked(){
	// SAVE TO QSETTINGS
	QSettings settings;

	settings.beginGroup("analysisFrequencies");
	settings.setValue("startingFrequencyA",pow(2,ui->stFreq->currentIndex())*27.5);
	settings.setValue("numOctaves",ui->octaves->value());
	settings.setValue("octaveBeginOffset",(ui->octaveOffset->isChecked() ? 3 : 0));
	settings.setValue("bandsPerSemitone",ui->bps->value());
	settings.endGroup();

	settings.beginGroup("spectralAnalysis");
  settings.setValue("temporalWindow",temporalWindowComboIndex[ui->temporalWindow->currentIndex()].toAscii());
	settings.setValue("fftFrameSize",pow(2,ui->fftFrameSize->currentIndex())*512);
	settings.setValue("hopSize",pow(2,ui->hopSize->currentIndex())*128);
	settings.setValue("directSkStretch",ui->directSkStretch->value());
	settings.endGroup();

	settings.beginGroup("downsampling");
	settings.setValue("dFactor",ui->dFactor->value());
	settings.endGroup();

	settings.beginGroup("tuning");
	settings.setValue("tuningMethod",ui->tuningMethod->currentIndex());
	settings.setValue("detunedBandWeight",ui->detunedBandWeight->value());
	settings.endGroup();

	settings.beginGroup("harmonicChangeDetectionFunction");
  settings.setValue("hcdf",hcdfComboIndex[ui->hcdf->currentIndex()].toAscii());
	settings.setValue("hcdfGaussianSize",ui->hcdfGaussianSize->value());
	settings.setValue("hcdfGaussianSigma",ui->hcdfGaussianSigma->value());
	settings.setValue("hcdfPeakPickingNeighbours",ui->hcdfPeakPickingNeighbours->value());
	settings.setValue("hcdfArbitrarySegments",ui->hcdfArbitrarySegments->value());
	settings.endGroup();

	settings.beginGroup("keyClassification");
	settings.setValue("toneProfile",ui->toneProfile->currentIndex());
  settings.setValue("similarityMeasure",similarityMeasureComboIndex[ui->similarityMeasure->currentIndex()].toAscii());
	settings.endGroup();

	settings.beginGroup("customToneProfile");
	settings.setValue("maj0",ui->maj0->value());
	settings.setValue("maj1",ui->maj1->value());
	settings.setValue("maj2",ui->maj2->value());
	settings.setValue("maj3",ui->maj3->value());
	settings.setValue("maj4",ui->maj4->value());
	settings.setValue("maj5",ui->maj5->value());
	settings.setValue("maj6",ui->maj6->value());
	settings.setValue("maj7",ui->maj7->value());
	settings.setValue("maj8",ui->maj8->value());
	settings.setValue("maj9",ui->maj9->value());
	settings.setValue("maj10",ui->maj10->value());
	settings.setValue("maj11",ui->maj11->value());
	settings.setValue("min0",ui->min0->value());
	settings.setValue("min1",ui->min1->value());
	settings.setValue("min2",ui->min2->value());
	settings.setValue("min3",ui->min3->value());
	settings.setValue("min4",ui->min4->value());
	settings.setValue("min5",ui->min5->value());
	settings.setValue("min6",ui->min6->value());
	settings.setValue("min7",ui->min7->value());
	settings.setValue("min8",ui->min8->value());
	settings.setValue("min9",ui->min9->value());
	settings.setValue("min10",ui->min10->value());
	settings.setValue("min11",ui->min11->value());
	settings.endGroup();

	settings.beginGroup("customKeyCodes");
	settings.setValue("majKey0",ui->majKey0->text());
	settings.setValue("majKey1",ui->majKey1->text());
	settings.setValue("majKey2",ui->majKey2->text());
	settings.setValue("majKey3",ui->majKey3->text());
	settings.setValue("majKey4",ui->majKey4->text());
	settings.setValue("majKey5",ui->majKey5->text());
	settings.setValue("majKey6",ui->majKey6->text());
	settings.setValue("majKey7",ui->majKey7->text());
	settings.setValue("majKey8",ui->majKey8->text());
	settings.setValue("majKey9",ui->majKey9->text());
	settings.setValue("majKey10",ui->majKey10->text());
	settings.setValue("majKey11",ui->majKey11->text());
	settings.setValue("minKey0",ui->minKey0->text());
	settings.setValue("minKey1",ui->minKey1->text());
	settings.setValue("minKey2",ui->minKey2->text());
	settings.setValue("minKey3",ui->minKey3->text());
	settings.setValue("minKey4",ui->minKey4->text());
	settings.setValue("minKey5",ui->minKey5->text());
	settings.setValue("minKey6",ui->minKey6->text());
	settings.setValue("minKey7",ui->minKey7->text());
	settings.setValue("minKey8",ui->minKey8->text());
	settings.setValue("minKey9",ui->minKey9->text());
	settings.setValue("minKey10",ui->minKey10->text());
	settings.setValue("minKey11",ui->minKey11->text());
	settings.setValue("silence",ui->silence->text());
	settings.endGroup();

  settings.beginGroup("tags");
  settings.setValue("tagFormat",tagFormatComboIndex[ui->tagFormat->currentIndex()].toAscii());
  settings.setValue("tagField",tagFieldComboIndex[ui->tagField->currentIndex()].toAscii());
  settings.endGroup();

	// CLOSE
	this->close();
}

void PrefsDialog::on_cancelButton_clicked(){
	this->close();
}

void PrefsDialog::tuningEnabled(){
	ui->tab2Tuning->setEnabled(ui->bps->value() > 1);
}

void PrefsDialog::binAdaptiveTuningEnabled(){
	ui->detunedBandWeight->setEnabled(ui->tuningMethod->currentIndex() == 1);
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

void PrefsDialog::on_bps_valueChanged(int /*arg1*/){
	tuningEnabled();
	binAdaptiveTuningEnabled();
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
