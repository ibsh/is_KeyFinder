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

#include "guidetail.h"
#include "ui_detailwindow.h"

const int ROW_BIGCHROMA = 0;
const int ROW_MINICHROMA = 1;
const int ROW_RATEOFCHANGE = 2;

const int PROGRESS_DONE = 0;
const int PROGRESS_START = 1;
const int PROGRESS_DECODED = 2;
const int PROGRESS_MADEMONO = 3;
const int PROGRESS_DOWNSAMPLED = 4;
const int PROGRESS_DONESPECTRUMANALYSIS = 5;
const int PROGRESS_DONEHARMONICANALYSIS = 6;

DetailWindow::DetailWindow(QWidget *parent, QString path) : QMainWindow(parent), ui(new Ui::DetailWindow){
	ui->setupUi(this);
	modelThread = NULL;
	allowDrops = true;
	vis = Visuals::getInstance();
	ui->progressBar->setMaximum(PROGRESS_DONEHARMONICANALYSIS);
	ui->progressBar->setVisible(false);
	ui->runButton->setDisabled(true);
	layoutScaling();
	drawColourScale();
	drawPianoKeys();
	blankVisualisations();
	blankKeyLabel();
	if(path != ""){
		filePath = path;
		processCurrentFile();
	}
}

DetailWindow::~DetailWindow(){
	if(modelThread!=NULL && modelThread->isRunning()){
		modelThread->quit();
		modelThread->wait();
	}
	delete ui;
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
	filePath = e->mimeData()->urls().at(0).toLocalFile();
	processCurrentFile();
}

void DetailWindow::processCurrentFile(){
	// get latest preferences and redraw variable UI elements if they've changed since the last run.
	int chkOctaves = prefs.getOctaves();
	int chkOffset = prefs.getOctaveOffset();
	prefs = Preferences();
	if(chkOctaves != prefs.getOctaves() || chkOffset != prefs.getOctaveOffset()){
		layoutScaling();
		drawPianoKeys();
	}
	// visuals
	say("Decoding audio... ");
	//blankVisualisations();
	//blankKeyLabel();
	ui->progressBar->setValue(PROGRESS_START);
	ui->progressBar->setVisible(true);
	ui->chromaColourCombo->setDisabled(true);
	ui->runButton->setDisabled(true);
	// and proceed
	modelThread = new KeyFinderWorkerThread(0);
	modelThread->setParams(filePath,prefs);
	connect(modelThread,SIGNAL(failed(QString)),this,SLOT(criticalError(QString)));
	connect(modelThread,SIGNAL(decoded()),this,SLOT(decoded()));
	connect(modelThread,SIGNAL(madeMono()),this,SLOT(madeMono()));
	connect(modelThread,SIGNAL(downsampled()),this,SLOT(downsampled()));
	connect(modelThread,SIGNAL(producedFullChromagram(Chromagram)),this,SLOT(receiveFullChromagram(Chromagram)));
	connect(modelThread,SIGNAL(producedOneOctaveChromagram(Chromagram)),this,SLOT(receiveOneOctaveChromagram(Chromagram)));
	connect(modelThread,SIGNAL(producedHarmonicChangeSignal(std::vector<double>)),this,SLOT(receiveHarmonicChangeSignal(std::vector<double>)));
	connect(modelThread,SIGNAL(producedKeyEstimates(std::vector<int>)),this,SLOT(receiveKeyEstimates(std::vector<int>)));
	connect(modelThread,SIGNAL(producedGlobalKeyEstimate(int)),this,SLOT(receiveGlobalKeyEstimate(int)));
	modelThread->start();
}

void DetailWindow::criticalError(const QString& s){
	say(s);
	cleanUpAfterRun();
}

void DetailWindow::decoded(){
	say("Decoded...");
	ui->progressBar->setValue(PROGRESS_DECODED);
}

void DetailWindow::madeMono(){
	say("Made mono...");
	ui->progressBar->setValue(PROGRESS_MADEMONO);
}

void DetailWindow::downsampled(){
	say("Downsampled...");
	ui->progressBar->setValue(PROGRESS_DOWNSAMPLED);
}

void DetailWindow::receiveFullChromagram(const Chromagram& ch){
	chromagramImage = imageFromChromagram(&ch);
	ui->chromagramLabel->setPixmap(QPixmap::fromImage(chromagramImage));
	ui->chromagramLabel->setMinimumHeight(ch.getBins()+2);
	ui->chromagramLabel->setMinimumWidth(ch.getHops()+2);
	// Tooltip
	QString numbers[] = {"","one octave","two octaves","three octaves","four octaves","five octaves","six octaves","seven octaves","eight octaves"};
	QString tooltip = "This chromagram spans " + numbers[prefs.getOctaves()] + ".\n";
	tooltip += "The vertical axis represents musical frequencies\nas indicated by the piano keyboard.\n";
	tooltip += "The horizontal axis splits the track into analysis\nwindows of about " + QString::number((44100.0/prefs.getDFactor())/prefs.getHopSize()).left(4) + " seconds each.\n";
	tooltip += "The brighter the colour, the higher the energy\nfound at that frequency.";
	ui->chromagramLabel->setToolTip(tooltip);
}

void DetailWindow::receiveOneOctaveChromagram(const Chromagram& ch){
	miniChromagramImage = imageFromChromagram(&ch);
	ui->miniChromagramLabel->setPixmap(QPixmap::fromImage(miniChromagramImage));
	ui->miniChromagramLabel->setToolTip("This is the same chromagram data,\ndecomposed to a single octave.");
	say("Spectrum analysis done...");
	ui->progressBar->setValue(PROGRESS_DONESPECTRUMANALYSIS);
}

void DetailWindow::receiveHarmonicChangeSignal(const std::vector<double>& rateOfChange){
	int rateOfChangePrecision = 100;
	harmonicChangeImage = QImage(rateOfChange.size()*chromaScaleH,rateOfChangePrecision,QImage::Format_Indexed8);
	vis->setImageColours(harmonicChangeImage,ui->chromaColourCombo->currentIndex());
	for(int h=0; h<(signed)rateOfChange.size(); h++){
		int value = rateOfChange[h] * rateOfChangePrecision;
		for(int y=0; y<rateOfChangePrecision; y++)
			for(int x=0; x<chromaScaleH; x++)
				harmonicChangeImage.setPixel(h*chromaScaleH+x, y, (rateOfChangePrecision - y > value ? 0 : 50));
	}
	ui->harmonicChangeLabel->setPixmap(QPixmap::fromImage(harmonicChangeImage));
	// Tooltip
	if(prefs.getHcdf() == 'n'){
		ui->harmonicChangeLabel->setToolTip("You are not using segmentation,\nso there is no harmonic change\ndata to display.");
	}else if(prefs.getHcdf() == 'a'){
		ui->harmonicChangeLabel->setToolTip("You are using arbitrary segmentation,\nso there is no harmonic change\ndata to display.");
	}else{
		ui->harmonicChangeLabel->setToolTip("This is the level of harmonic\nchange detected in the\nchromagram over time. Peaks\nin this signal are used to\nsegment the chromagram.");
	}
}

void DetailWindow::receiveKeyEstimates(const std::vector<int>& keys){
	deleteKeyLabels();
	int lastChange = 0;
	for(int h=1; h<(signed)keys.size(); h++){ // don't test the first hop
		if(h==(signed)keys.size()-1 || (keys[h] != keys[h-1])){ // at the end, and at changes
			QLabel* newLabel = new QLabel(vis->getKeyName(keys[h-1]));
			newLabel->setAlignment(Qt::AlignCenter);
			QPalette pal = newLabel->palette();
			pal.setColor(backgroundRole(),vis->getKeyColour(keys[h-1]));
			newLabel->setPalette(pal);
			newLabel->setFrameStyle(1);
			newLabel->setAutoFillBackground(true);
			newLabel->setMinimumHeight(20);
			newLabel->setMaximumHeight(30);
			if(prefs.getHcdf() == 'n'){
				newLabel->setToolTip("This row shows the key estimate for\nthe unsegmented chromagram.");
			}else if(prefs.getHcdf() == 'a'){
				newLabel->setToolTip("This row shows the key estimates\nfor the arbitrary segments.");
			}else{
				newLabel->setToolTip("This row shows the key estimates\nfor the segments between peak\nharmonic changes.");
			}
			ui->horizontalLayout_keyLabels->addWidget(newLabel,h-lastChange);
			keyLabels.push_back(newLabel);
			lastChange = h;
		}
	}
}

void DetailWindow::receiveGlobalKeyEstimate(int key){
	TagLibMetadata md(filePath);
	QString shortName = md.getTitle();
	if(shortName == ""){
		shortName = filePath.right(filePath.length() - filePath.lastIndexOf("/") - 1);
	}
	this->setWindowTitle("KeyFinder - Detailed Analysis - " + shortName);
	say("Key estimate: " + vis->getKeyName(key));
	cleanUpAfterRun();
}

void DetailWindow::cleanUpAfterRun(){
	delete modelThread;
	modelThread = NULL;
	ui->progressBar->setValue(PROGRESS_DONE);
	ui->progressBar->setVisible(false);
	ui->chromaColourCombo->setDisabled(false);
	ui->runButton->setDisabled(false);
	allowDrops = true;
}

QImage DetailWindow::imageFromChromagram(const Chromagram* ch){
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

void DetailWindow::on_runButton_clicked(){
	if(filePath != "") processCurrentFile();
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

void DetailWindow::layoutScaling(){
	ui->gridLayout_Visualisation->setRowStretch(ROW_BIGCHROMA,prefs.getOctaves()*2);
	ui->gridLayout_Visualisation->setRowStretch(ROW_MINICHROMA,2);
	ui->gridLayout_Visualisation->setRowStretch(ROW_RATEOFCHANGE,1);
	chromaScaleV = 5;
	chromaScaleH = 5*(prefs.getHopSize()/16384.0)*(prefs.getDFactor()/10.0);
	if(chromaScaleH < 1) chromaScaleH = 1;
}

void DetailWindow::drawColourScale(){
	colourScaleImage = QImage(1,65,QImage::Format_Indexed8);
	vis->setImageColours(colourScaleImage,ui->chromaColourCombo->currentIndex());
	for(int i=0; i<=64; i++)
		colourScaleImage.setPixel(0,64-i,i);
	ui->colourScaleLabel->setPixmap(QPixmap::fromImage(colourScaleImage));
}

void DetailWindow::blankVisualisations(){
	chromagramImage = QImage(1,1,QImage::Format_Indexed8);
	miniChromagramImage = QImage(1,1,QImage::Format_Indexed8);
	harmonicChangeImage = QImage(1,1,QImage::Format_Indexed8);
	vis->setImageColours(chromagramImage,ui->chromaColourCombo->currentIndex());
	vis->setImageColours(miniChromagramImage,ui->chromaColourCombo->currentIndex());
	vis->setImageColours(harmonicChangeImage,ui->chromaColourCombo->currentIndex());
	chromagramImage.setPixel(0,0,0);
	miniChromagramImage.setPixel(0,0,0);
	harmonicChangeImage.setPixel(0,0,0);
	ui->chromagramLabel->setPixmap(QPixmap::fromImage(chromagramImage));
	ui->miniChromagramLabel->setPixmap(QPixmap::fromImage(miniChromagramImage));
	ui->harmonicChangeLabel->setPixmap(QPixmap::fromImage(harmonicChangeImage));
	QString blank = "Drag an audio file onto the window.";
	ui->chromagramLabel->setToolTip(blank);
	ui->miniChromagramLabel->setToolTip(blank);
	ui->harmonicChangeLabel->setToolTip(blank);
}

void DetailWindow::deleteKeyLabels(){
	for(int i=keyLabels.size()-1; i>=0; i--){
		delete keyLabels[i];
		keyLabels.pop_back();
	}
}

void DetailWindow::blankKeyLabel(){
	deleteKeyLabels();
	QLabel* dummyLabel = new QLabel();
	QPalette pal = dummyLabel->palette();
	pal.setColor(backgroundRole(), Qt::black);
	dummyLabel->setPalette(pal);
	dummyLabel->setAutoFillBackground(true);
	dummyLabel->setMinimumHeight(20);
	dummyLabel->setMaximumHeight(30);
	dummyLabel->setToolTip("Drag an audio file onto the window.");
	ui->horizontalLayout_keyLabels->addWidget(dummyLabel);
	keyLabels.push_back(dummyLabel);
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
	// reverse of octave for visual representation (ending at A by default)
	QString octaveRev = "bwbwwbwbwwbw";
	int off = prefs.getOctaveOffset();
	if(off > 0)
		octaveRev = octaveRev.right(off) + octaveRev.left(12-off);
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
