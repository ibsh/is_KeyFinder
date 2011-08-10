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

#include "prefsdialog.h"
#include "ui_prefsdialog.h"

PrefsDialog::PrefsDialog(QWidget *parent): QDialog(parent),ui(new Ui::PrefsDialog){
	ui->setupUi(this);
	this->setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

	// initialise Preferences object to ensure QSettings is written
	Preferences prefsObj;

	// these strings store the chars relating to each index of their dropdowns
	temporalWindowComboIndex = "bmn";
	spectrumAnalyerComboIndex = "fg";
	fftPostProcessorComboIndex = "ci";
	hcdfComboIndex = "nhc";

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
	ui->spectrumAnalyser->setCurrentIndex(spectrumAnalyerComboIndex.indexOf(settings.value("spectrumAnalyser").toChar()));
	ui->fftPostProcessor->setCurrentIndex(fftPostProcessorComboIndex.indexOf(settings.value("fftPostProcessor").toChar()));
	ui->fftFrameSize->setCurrentIndex(log2(settings.value("fftFrameSize").toInt()/4096));
	ui->hopSize->setCurrentIndex(log2(settings.value("hopSize").toInt()/1024));
	ui->directSkStretch->setValue(settings.value("directSkStretch").toFloat());
	ui->goertzelMinK->setValue(settings.value("goertzelMinK").toInt());
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
	settings.endGroup();

	settings.beginGroup("keyClassification");
	ui->toneProfile ->setCurrentIndex(settings.value("toneProfile").toInt());
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

	// enable/disable fields as necessary
	fftEnabled();
	goertzelEnabled();
	directSkEnabled();
	tuningEnabled();
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
	settings.setValue("spectrumAnalyser",spectrumAnalyerComboIndex[ui->spectrumAnalyser->currentIndex()].toAscii());
	settings.setValue("fftPostProcessor",fftPostProcessorComboIndex[ui->fftPostProcessor->currentIndex()].toAscii());
	settings.setValue("fftFrameSize",pow(2,ui->fftFrameSize->currentIndex())*4096);
	settings.setValue("hopSize",pow(2,ui->hopSize->currentIndex())*1024);
	settings.setValue("directSkStretch",ui->directSkStretch->value());
	settings.setValue("goertzelMinK",ui->goertzelMinK->value());
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
	settings.endGroup();

	settings.beginGroup("keyClassification");
	settings.setValue("toneProfile",ui->toneProfile->currentIndex());
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

	// CLOSE
	this->close();
}

void PrefsDialog::on_cancelButton_clicked(){
	this->close();
}

void PrefsDialog::fftEnabled(){
	bool e = (ui->spectrumAnalyser->currentIndex() == 0);
	ui->fftFrameSize->setEnabled(e);
	ui->fftPostProcessor->setEnabled(e);
}

void PrefsDialog::goertzelEnabled(){
	ui->goertzelMinK->setEnabled(ui->spectrumAnalyser->currentIndex() == 1);
	ui->directSkStretch->setEnabled(!ui->goertzelMinK->isEnabled());
}

void PrefsDialog::directSkEnabled(){
	ui->directSkStretch->setEnabled(ui->fftPostProcessor->currentIndex() == 1);
}

void PrefsDialog::tuningEnabled(){
	ui->tab2Tuning->setEnabled(ui->bps->value() > 1);
}

void PrefsDialog::hcdfEnabled(){
	bool e = (ui->hcdf->currentIndex() != 0);
	ui->hcdfGaussianSigma->setEnabled(e);
	ui->hcdfGaussianSize->setEnabled(e);
	ui->hcdfPeakPickingNeighbours->setEnabled(e);
}

void PrefsDialog::customProfileEnabled(){
	bool e = (ui->toneProfile->currentIndex() == 3);
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

void PrefsDialog::on_spectrumAnalyser_currentIndexChanged(int index){
	fftEnabled();
	goertzelEnabled();
}

void PrefsDialog::on_fftPostProcessor_currentIndexChanged(int index){
	directSkEnabled();
}

void PrefsDialog::on_bps_valueChanged(int arg1){
	tuningEnabled();
}

void PrefsDialog::on_hcdf_currentIndexChanged(int index){
	hcdfEnabled();
}

void PrefsDialog::on_toneProfile_currentIndexChanged(int index){
	customProfileEnabled();
}
