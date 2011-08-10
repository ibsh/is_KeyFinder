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

#include "detailwindow.h"
#include "ui_detailwindow.h"

const int ROW_BIGCHROMA = 0;
const int ROW_MINICHROMA = 1;
const int ROW_RATEOFCHANGE = 2;

DetailWindow::DetailWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::DetailWindow){
	// SETUP KEYFINDER
	ab = NULL;
	sa = NULL;
	ch = NULL;
	// SETUP ASYNC SIGNALS/SLOTS (do it once here so they don't repeat if a decode fails).
	connect(&decodeWatcher, SIGNAL(finished()), this, SLOT(decoded()));
	connect(&monoWatcher, SIGNAL(finished()), this, SLOT(madeMono()));
	connect(&dsWatcher, SIGNAL(finished()), this, SLOT(downSampled()));
	connect(&saWatcher, SIGNAL(finished()), this, SLOT(spectrumAnalysisComplete()));
	connect(&haWatcher, SIGNAL(finished()), this, SLOT(haFinished()));
	// SETUP UI
	ui->setupUi(this);
	allowDrops = true;
	vis = Visuals::getInstance();
	ui->progressBar->setMaximum(5);
	ui->progressBar->setVisible(false);
	ui->runButton->setDisabled(true);
	ui->gridLayout_Visualisation->setRowStretch(ROW_BIGCHROMA,prefs.getOctaves()*2);
	ui->gridLayout_Visualisation->setRowStretch(ROW_MINICHROMA,2);
	ui->gridLayout_Visualisation->setRowStretch(ROW_RATEOFCHANGE,1);
	// DEFAULT FOR PARAMETER DROPDOWN
	ui->chromaColourCombo->setCurrentIndex(3);			// 3 = Hack
	// VISUALISATION IMAGES
	chromaScaleV = 5;
	chromaScaleH = 5*(prefs.getHopSize()/16384.0)*(prefs.getDFactor()/10.0);
	if(chromaScaleH < 1) chromaScaleH = 1;
	chromagramImage = QImage(1,1,QImage::Format_Indexed8);
	miniChromagramImage = QImage(1,1,QImage::Format_Indexed8);
	harmonicChangeImage = QImage(1,1,QImage::Format_Indexed8);
	colourScaleImage = QImage(1,65,QImage::Format_Indexed8);
	vis->setImageColours(chromagramImage,ui->chromaColourCombo->currentIndex());
	vis->setImageColours(miniChromagramImage,ui->chromaColourCombo->currentIndex());
	vis->setImageColours(harmonicChangeImage,ui->chromaColourCombo->currentIndex());
	vis->setImageColours(colourScaleImage,ui->chromaColourCombo->currentIndex());
	chromagramImage.setPixel(0,0,0);
	miniChromagramImage.setPixel(0,0,0);
	harmonicChangeImage.setPixel(0,0,0);
	for(int i=0; i<=64; i++)
		colourScaleImage.setPixel(0,64-i,i);
	ui->chromagramLabel->setPixmap(QPixmap::fromImage(chromagramImage));
	ui->miniChromagramLabel->setPixmap(QPixmap::fromImage(miniChromagramImage));
	ui->harmonicChangeLabel->setPixmap(QPixmap::fromImage(harmonicChangeImage));
	ui->colourScaleLabel->setPixmap(QPixmap::fromImage(colourScaleImage));
	drawPianoKeys();
	// DUMMY KEY LABEL
	QLabel* dummyLabel = new QLabel();
	QPalette pal = dummyLabel->palette();
	pal.setColor(backgroundRole(), Qt::black);
	dummyLabel->setPalette(pal);
	dummyLabel->setAutoFillBackground(true);
	dummyLabel->setMinimumHeight(20);
	dummyLabel->setMaximumHeight(30);
	dummyLabel->setToolTip("This row shows the key(s) detected\nin the segments between peak\nharmonic changes.");
	ui->horizontalLayout_keyLabels->addWidget(dummyLabel);
	keyLabels.push_back(dummyLabel);
}

void DetailWindow::dragEnterEvent(QDragEnterEvent *e){
	// accept only single, local files
	if(
		allowDrops
		&& e->mimeData()->hasUrls()
		&& e->mimeData()->urls().size() == 1
		&& !e->mimeData()->urls().at(0).toLocalFile().isEmpty()
	){
		e->acceptProposedAction();
	}
}

void DetailWindow::dropEvent(QDropEvent *e){
	allowDrops = false;
	filePath = (std::string)e->mimeData()->urls().at(0).toLocalFile().toAscii();
	go();
}

void DetailWindow::go(){
	// get latest preferences and redraw, just in case they've changed since the last run.
	prefs = Preferences();
	ui->gridLayout_Visualisation->setRowStretch(ROW_BIGCHROMA,prefs.getOctaves()*2);
	chromaScaleH = 5*(prefs.getHopSize()/16384.0)*(prefs.getDFactor()/10.0);
	drawPianoKeys();
	// now proceed
	say("Decoding audio... ");
	ui->progressBar->setValue(1);
	ui->progressBar->setVisible(true);
	ui->chromaColourCombo->setDisabled(true);
	ui->runButton->setDisabled(true);
	// Begin asynchronous process
	QFuture<void> future = QtConcurrent::run(this,&DetailWindow::decode);
	decodeWatcher.setFuture(future);
}

void DetailWindow::cleanUpAfterRun(){
	ui->progressBar->setValue(0);
	ui->progressBar->setVisible(false);
	ui->chromaColourCombo->setDisabled(false);
	ui->runButton->setDisabled(false);
	allowDrops = true;
}

void DetailWindow::decode(){
	AudioFileDecoder* dec = AudioFileDecoder::getDecoder(filePath);
	try{
		ab = dec->decodeFile((char*)filePath.c_str());
		delete dec;
	}catch(Exception){
		delete ab;
		ab = NULL;
		delete dec;
	}
}

void DetailWindow::decoded(){
	if(ab==NULL){
		say("Decode failed. Try another file.");
		cleanUpAfterRun();
		return;
	}
	say("Decoded, making mono...");
	ui->progressBar->setValue(2);
	QFuture<void> future = QtConcurrent::run(this,&DetailWindow::makeMono);
	monoWatcher.setFuture(future);
}

void DetailWindow::makeMono(){
	Monaural* mono = new Monaural();
	ab = mono->makeMono(ab);
	delete mono;
}

void DetailWindow::madeMono(){
	if(prefs.getDFactor() < 2){
		say("Made mono. Running spectrum analysis...");
		ui->progressBar->setValue(4);
		QFuture<void> future = QtConcurrent::run(this,&DetailWindow::spectrumAnalysis);
		saWatcher.setFuture(future);
	}else{
		say("Made mono. Downsampling... ");
		ui->progressBar->setValue(3);
		QFuture<void> future = QtConcurrent::run(this,&DetailWindow::downSample);
		dsWatcher.setFuture(future);
	}
}

void DetailWindow::downSample(){
	Downsampler* ds = Downsampler::getDownsampler(prefs.getDFactor(),ab->getFrameRate(),prefs.getLastFreq());
	try{
		ab = ds->downsample(ab,prefs.getDFactor());
		delete ds;
	}catch(Exception){
		delete ab;
		ab = NULL;
		delete ds;
	}
}

void DetailWindow::downSampled(){
	if(ab==NULL){
		say("Downsample failed. Try again.");
		cleanUpAfterRun();
		return;
	}
	say("Downsampled. Running spectrum analysis...");
	ui->progressBar->setValue(4);
	QFuture<void> future = QtConcurrent::run(this,&DetailWindow::spectrumAnalysis);
	saWatcher.setFuture(future);
}

void DetailWindow::spectrumAnalysis(){
	sa = SpectrumAnalyserFactory::getInstance()->getSpectrumAnalyser(ab->getFrameRate(),prefs);
	ch = sa->chromagram(ab);
	ch->decomposeToTwelveBpo(prefs);
}

void DetailWindow::spectrumAnalysisComplete(){
	// paint full chromagram
	chromagramImage = imageFromChromagram(ch);
	ui->chromagramLabel->setPixmap(QPixmap::fromImage(chromagramImage));
	ui->chromagramLabel->setMinimumHeight(ch->getBins()+2);
	ui->chromagramLabel->setMinimumWidth(ch->getHops()+2);
	say("Running harmonic analyses...");
	ui->progressBar->setValue(5);
	QFuture<void> future = QtConcurrent::run(this,&DetailWindow::harmonicAnalysis);
	haWatcher.setFuture(future);
}

void DetailWindow::harmonicAnalysis(){
	ch->decomposeToOneOctave(prefs);
	Hcdf* hcdf = Hcdf::getHcdf(prefs);
	rateOfChange = hcdf->hcdf(ch,prefs);
	std::vector<int> changes = hcdf->peaks(rateOfChange,prefs);
	changes.push_back(ch->getHops()); // add sentinel
	KeyClassifier hc(prefs);
	keys = std::vector<int>(0);
	for(int i=0; i<(signed)changes.size()-1; i++){
		std::vector<double> chroma(12);
		for(int j=changes[i]; j<changes[i+1]; j++)
			for(int k=0; k<ch->getBins(); k++)
				chroma[k] += ch->getMagnitude(j,k);
		int key = hc.classify(chroma);
		for(int j=changes[i]; j<changes[i+1]; j++)
			keys.push_back(key);
	}
	keys.push_back(keys[keys.size()-1]); // put last key on again to match length of track
}

void DetailWindow::haFinished(){
	// paint single octave chromagram
	miniChromagramImage = imageFromChromagram(ch);
	ui->miniChromagramLabel->setPixmap(QPixmap::fromImage(miniChromagramImage));
	// paint rate of change
	int rateOfChangePrecision = 100;
	harmonicChangeImage = QImage(rateOfChange.size()*chromaScaleH,rateOfChangePrecision,QImage::Format_Indexed8);
	vis->setImageColours(harmonicChangeImage,ui->chromaColourCombo->currentIndex());
	// set pixels
	for(int h=0; h<(signed)rateOfChange.size(); h++){
		int value = rateOfChange[h] * rateOfChangePrecision;
		for(int y=0; y<rateOfChangePrecision; y++)
			for(int x=0; x<chromaScaleH; x++)
				harmonicChangeImage.setPixel(h*chromaScaleH+x, y, (rateOfChangePrecision - y > value ? 0 : 50));
	}
	// show
	ui->harmonicChangeLabel->setPixmap(QPixmap::fromImage(harmonicChangeImage));
	// Key labels
	for(int i=keyLabels.size()-1; i>=0; i--){
		delete keyLabels[i];
		keyLabels.pop_back();
	}
	int lastChange = 0;
	for(int h=1; h<(signed)keys.size(); h++){ // don't test the first hop
		if(h==(signed)keys.size()-1 || (keys[h] != keys[h-1])){ // at the end, and at changes
			QLabel* newLabel = new QLabel(vis->keyNames[keys[h-1]]);
			newLabel->setAlignment(Qt::AlignCenter);
			QPalette pal = newLabel->palette();
			pal.setColor(backgroundRole(),vis->keyColours[keys[h-1]]);
			newLabel->setPalette(pal);
			newLabel->setFrameStyle(1);
			newLabel->setAutoFillBackground(true);
			newLabel->setMinimumHeight(20);
			newLabel->setMaximumHeight(30);
			newLabel->setToolTip("This row shows the key(s) detected\nin the segments between peak\nharmonic changes.");
			ui->horizontalLayout_keyLabels->addWidget(newLabel,h-lastChange);
			keyLabels.push_back(newLabel);
			lastChange = h;
		}
	}
	// text
	Metadata* md = Metadata::getMetadata((char*)filePath.c_str());
	QString shortName;
	shortName = QString::fromUtf8(md->getTitle().c_str());
	if(shortName == ""){
		shortName = QString::fromUtf8(filePath.substr(filePath.rfind("/")+1).c_str());
	}
	this->setWindowTitle("KeyFinder - Detailed Analysis - " + shortName);
	say(shortName + " - analysis complete.");
	delete ch;
	ch = NULL;
	cleanUpAfterRun();
}

void DetailWindow::drawPianoKeys(){
	int scale = 10;
	QImage pianoImage = QImage(1,prefs.getOctaves()*12*scale,QImage::Format_Indexed8);
	QImage miniPianoImage = QImage(1,12*scale,QImage::Format_Indexed8);
	pianoImage.setColor(0,qRgb(255,255,255));	// white key
	pianoImage.setColor(1,qRgb(0,0,0));				// black key
	pianoImage.setColor(2,qRgb(127,127,127));	// boundary colour
	miniPianoImage.setColor(0,qRgb(255,255,255));
	miniPianoImage.setColor(1,qRgb(0,0,0));
	miniPianoImage.setColor(2,qRgb(127,127,127));
	// reverse of octave for visual representation (ending at A; don't forget the offset)
	std::string octaveRev = "bwbwwbwbwwbw";
	int off = prefs.getOctaveOffset();
	if(off > 0)
		octaveRev = octaveRev.substr(12-off,off) + octaveRev.substr(0,12 - off);
	for(int o=0; o<prefs.getOctaves(); o++){
		for(int s=0; s<12; s++){
			for(int px=0; px<scale-1; px++){
				pianoImage.setPixel(0,(o*12*scale)+(s*scale)+px,(octaveRev[s] == 'b' ? 1 : 0));
				miniPianoImage.setPixel(0,(s*scale)+px,(octaveRev[s] == 'b' ? 1 : 0));
			}
			pianoImage.setPixel(0,(o*12*scale)+(s*scale)+scale-1,2);
			miniPianoImage.setPixel(0,(s*scale)+scale-1,2);
		}
	}
	ui->pianoKeysLabel->setPixmap(QPixmap::fromImage(pianoImage));
	ui->miniPianoKeysLabel->setPixmap(QPixmap::fromImage(miniPianoImage));
}

QImage DetailWindow::imageFromChromagram(Chromagram* ch){
	// 64 colours (plus black at index 0)
	// don't draw individual pixels; draw blocks of chromaScaleV*chromaScaleH. Sharpens image.
	QImage img = QImage(ch->getHops()*chromaScaleH,ch->getBins()*chromaScaleV,QImage::Format_Indexed8);
	vis->setImageColours(img,ui->chromaColourCombo->currentIndex());
	// get max to normalise
	float max = 0;
	for(int h=0; h<ch->getHops(); h++){
		for(int b=0; b<ch->getBins(); b++){
			float mag = ch->getMagnitude(h,b);
			if(mag>max) max = mag;
		}
	}
	// set pixels
	for(int h=0; h<ch->getHops(); h++){
		for(int b=0; b<ch->getBins(); b++){
			int pixVal = ch->getMagnitude(h,b) / max * img.colorCount() - 1;
			if(pixVal<1)
				pixVal = 1;
			for(int x=0; x<chromaScaleH; x++)
				for(int y=0; y<chromaScaleV; y++)
					img.setPixel(h*chromaScaleH+x, (ch->getBins()-1-b)*chromaScaleV+y, pixVal);
		}
	}
	return img;
}

void DetailWindow::say(const QString& s){
	ui->statusLabel->setText(s);
}

DetailWindow::~DetailWindow(){
	decodeWatcher.cancel();
	decodeWatcher.waitForFinished();
	monoWatcher.cancel();
	monoWatcher.waitForFinished();
	dsWatcher.cancel();
	dsWatcher.waitForFinished();
	saWatcher.cancel();
	saWatcher.waitForFinished();
	haWatcher.cancel();
	haWatcher.waitForFinished();
	delete ui;
}

void DetailWindow::on_runButton_clicked(){
	if(filePath != "") go();
}

void DetailWindow::analyse(std::string path){
	// public slot to be called from batch window
	filePath = path;
	go();
}

void DetailWindow::on_chromaColourCombo_currentIndexChanged(int index){
	vis->setImageColours(chromagramImage,index);
	vis->setImageColours(miniChromagramImage,index);
	vis->setImageColours(harmonicChangeImage,index);
	vis->setImageColours(colourScaleImage,index);
	ui->chromagramLabel->setPixmap(QPixmap::fromImage(chromagramImage));
	ui->miniChromagramLabel->setPixmap(QPixmap::fromImage(miniChromagramImage));
	ui->harmonicChangeLabel->setPixmap(QPixmap::fromImage(harmonicChangeImage));
	ui->colourScaleLabel->setPixmap(QPixmap::fromImage(colourScaleImage));
}
