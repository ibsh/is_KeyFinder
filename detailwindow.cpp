#include "detailwindow.h"
#include "ui_detailwindow.h"

DetailWindow::DetailWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::DetailWindow){
	// SETUP KEYFINDER
	ab = NULL;
	sa = NULL;
	ch = NULL;
	prefs.setSpectrumAnalyser('f');
	prefs.setFftPostProcessor('i');
	prefs.setTemporalWindow('b');
	prefs.setToneProfile(2);
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
	chromagramImage = QImage(1,1,QImage::Format_Indexed8);
	miniChromagramImage = QImage(1,1,QImage::Format_Indexed8);
	colourScaleImage = QImage(1,65,QImage::Format_Indexed8);
	vis->setChromagramColours(chromagramImage,0);
	vis->setChromagramColours(miniChromagramImage,0);
	vis->setChromagramColours(colourScaleImage,0);
	chromagramImage.setPixel(0,0,1);
	miniChromagramImage.setPixel(0,0,1);
	for(int i=1; i<=65; i++)
		colourScaleImage.setPixel(0,65-i,i);
	ui->chromagramLabel->setPixmap(QPixmap::fromImage(chromagramImage));
	ui->miniChromagramLabel->setPixmap(QPixmap::fromImage(miniChromagramImage));
	ui->colourScaleLabel->setPixmap(QPixmap::fromImage(colourScaleImage));
	ui->gridLayout_Visualisation->setRowStretch(0,prefs.getOctaves());
	drawPianoKeys();
	// key labels
	QLabel* newLabel = new QLabel();
	QPalette pal = newLabel->palette();
	pal.setColor(backgroundRole(), Qt::black);
	newLabel->setPalette(pal);
	newLabel->setAutoFillBackground(true);
	newLabel->setMinimumHeight(20);
	newLabel->setMaximumHeight(30);
	ui->horizontalLayout_keyLabels->addWidget(newLabel);
	keyLabels.push_back(newLabel);
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
	say("Decoding audio... ");
	ui->progressBar->setValue(1);
	ui->progressBar->setVisible(true);
	ui->spectrumAnalyserCombo->setDisabled(true);
	ui->temporalWindowCombo->setDisabled(true);
	ui->toneProfilesCombo->setDisabled(true);
	ui->chromaColourCombo->setDisabled(true);
	ui->runButton->setDisabled(true);
	// Asynchronous, threaded computation
	QFuture<void> future = QtConcurrent::run(this,&DetailWindow::decode);
	decodeWatcher.setFuture(future);
}

void DetailWindow::cleanUpAfterRun(){
	ui->progressBar->setValue(0);
	ui->progressBar->setVisible(false);
	ui->spectrumAnalyserCombo->setDisabled(false);
	ui->temporalWindowCombo->setDisabled(false);
	ui->toneProfilesCombo->setDisabled(false);
	ui->chromaColourCombo->setDisabled(false);
	ui->runButton->setDisabled(false);
	allowDrops = true;
}

void DetailWindow::decode(){
	AudioFileDecoder* dec = AudioFileDecoder::getDecoder(filePath);
	try{
		ab = dec->decodeFile((char*)filePath.c_str());
		delete dec;
	}catch(const Exception& e){
		delete ab;
		delete dec;
		ab = NULL;
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
	Downsampler* ds = Downsampler::getDownsampler(prefs.getDFactor(),ab->getFrameRate(),prefs.getBinFreq(-1));
	try{
		ab = ds->downsample(ab,prefs.getDFactor());
		delete ds;
	}catch(const Exception& e){
		delete ab;
		delete ds;
		ab = NULL;
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
	// Paint 12-bpo chroma to screen in 64 colours (plus black at index 1)
	int bins = prefs.getOctaves()*12;
	int scale = 5; // don't draw individual pixels; draw blocks of scale*scale. Sharpens image.
	chromagramImage = QImage(ch->getHops()*scale,bins*scale,QImage::Format_Indexed8);
	vis->setChromagramColours(chromagramImage,ui->chromaColourCombo->currentIndex());
	// get max to normalise
	float max = 0;
	std::vector<std::vector<float> > chromaBuilder(ch->getHops(),std::vector<float>(ch->getBins()));
	for(int h=0; h<ch->getHops(); h++){
		for(int b=0; b<ch->getBins(); b++){
			float mag = ch->getMagnitude(h,b);
			chromaBuilder[h][b] = mag;
			if(mag>max) max = mag;
		}
	}
	// set pixels
	for(int h=0; h<ch->getHops(); h++){
		for(int b=0; b<ch->getBins(); b++){
			int pixVal =((chromaBuilder[h][b] / max) * (chromagramImage.colorCount()-2)) + 1;
			if(pixVal<1)
				pixVal = 1;
			for(int x=0; x<scale; x++)
				for(int y=0; y<scale; y++)
					chromagramImage.setPixel(h*scale+x, (bins-1-b)*scale+y, pixVal);
		}
	}
	// show
	ui->chromagramLabel->setPixmap(QPixmap::fromImage(chromagramImage));
	ui->chromagramLabel->setMinimumHeight(bins+2);
	ui->chromagramLabel->setMinimumWidth(ch->getHops()+2);
	say("Running harmonic analyses...");
	ui->progressBar->setValue(5);
	QFuture<void> future = QtConcurrent::run(this,&DetailWindow::harmonicAnalysis);
	haWatcher.setFuture(future);
}

void DetailWindow::harmonicAnalysis(){
	ch->decomposeToOneOctave(prefs);
	Hcdf harto;
	std::vector<int> changes = harto.hcdf(ch,prefs);
	changes.push_back(ch->getHops()); // add sentinel to back end
	KeyClassifier hc(prefs.getToneProfile());
	keys = std::vector<int>(0);
	for(int i=0; i<changes.size()-1; i++){
		std::vector<double> chroma(12);
		for(int j=changes[i]; j<changes[i+1]; j++){
			for(int k=0; k<ch->getBins(); k++){
				chroma[k] += ch->getMagnitude(j,k);
			}
		}
		int key = hc.classify(chroma);
		for(int j=changes[i]; j<changes[i+1]; j++){
			keys.push_back(key);
		}
	}
	keys.push_back(keys[keys.size()-1]); // put last key on again to match length of track
}

void DetailWindow::haFinished(){
	// draw single octave chromagram
	int scale = 5; // don't draw individual pixels; draw blocks of scale*scale. Sharpens image.
	int bins = 12;
	miniChromagramImage = QImage(ch->getHops()*scale,bins*scale,QImage::Format_Indexed8);
	vis->setChromagramColours(miniChromagramImage,ui->chromaColourCombo->currentIndex());
	// get max to normalise
	float max = 0;
	std::vector<std::vector<float> > chromaBuilder(ch->getHops(),std::vector<float>(ch->getBins()));
	for(int h=0; h<ch->getHops(); h++){
		for(int b=0; b<ch->getBins(); b++){
			float mag = ch->getMagnitude(h,b);
			chromaBuilder[h][b] = mag;
			if(mag>max) max = mag;
		}
	}
	// set pixels
	for(int h=0; h<ch->getHops(); h++){
		for(int b=0; b<ch->getBins(); b++){
			int pixVal =((chromaBuilder[h][b] / max) * (miniChromagramImage.colorCount()-2)) + 1;
			if(pixVal<1)
				pixVal = 1;
			for(int x=0; x<scale; x++)
				for(int y=0; y<scale; y++)
					miniChromagramImage.setPixel(h*scale+x, (bins-1-b)*scale+y, pixVal);
		}
	}
	// show
	ui->miniChromagramLabel->setPixmap(QPixmap::fromImage(miniChromagramImage));
	// Key labels
	for(int i=keyLabels.size()-1; i>=0; i--){
		delete keyLabels[i];
		keyLabels.pop_back();
	}
	int lastChange = 0;
	for(int h=0; h<keys.size(); h++){
		if(h==keys.size()-1 || (h != 0 && keys[h] != keys[h-1])){
			QLabel* newLabel = new QLabel(vis->keyNames[keys[h-1]]);
			newLabel->setAlignment(Qt::AlignCenter);
			QPalette pal = newLabel->palette();
			pal.setColor(backgroundRole(),vis->keyColours[keys[h-1]]);
			newLabel->setPalette(pal);
			newLabel->setFrameStyle(1);
			newLabel->setAutoFillBackground(true);
			newLabel->setMinimumHeight(20);
			newLabel->setMaximumHeight(30);
			ui->horizontalLayout_keyLabels->addWidget(newLabel,h-lastChange);
			keyLabels.push_back(newLabel);
			lastChange = h;
		}
	}
	// finale
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
	pianoImage.setColor(0,qRgb(255,255,255));
	miniPianoImage.setColor(0,qRgb(255,255,255));
	pianoImage.setColor(1,qRgb(0,0,0));
	miniPianoImage.setColor(1,qRgb(0,0,0));
	pianoImage.setColor(2,qRgb(127,127,127));
	miniPianoImage.setColor(2,qRgb(127,127,127));
	QString octaveRev = "bwbwwbwbwwbw";
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

void DetailWindow::say(const QString& s){
	ui->statusLabel->setText(s);
}

void DetailWindow::on_actionNew_Detail_Keyfinder_triggered(){
	DetailWindow* newWin = new DetailWindow(0);
	newWin->show();
}

void DetailWindow::on_actionNew_Batch_Keyfinder_triggered(){
	BatchWindow* newWin = new BatchWindow(0);
	newWin->show();
}

void DetailWindow::on_actionClose_Window_triggered(){
	this->close();
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

void DetailWindow::on_spectrumAnalyserCombo_currentIndexChanged(int index){
	if(index == 0){
		prefs.setSpectrumAnalyser('g');
	}
	if(index == 1){
		prefs.setSpectrumAnalyser('f');
		prefs.setFftPostProcessor('l');
	}
	if(index == 2){
		prefs.setSpectrumAnalyser('f');
		prefs.setFftPostProcessor('c');
	}
	if(index == 3){
		prefs.setSpectrumAnalyser('f');
		prefs.setFftPostProcessor('i');
	}
}

void DetailWindow::on_temporalWindowCombo_currentIndexChanged(int index){
	if(index == 0){
		prefs.setTemporalWindow('b');
	}
	if(index == 1){
		prefs.setTemporalWindow('m');
	}
	if(index == 2){
		prefs.setTemporalWindow('n');
	}
}

void DetailWindow::on_toneProfilesCombo_currentIndexChanged(int index){
	prefs.setToneProfile(index);
}

void DetailWindow::on_runButton_clicked(){
	if(filePath != "") go();
}

void DetailWindow::on_chromaColourCombo_currentIndexChanged(int index){
	vis->setChromagramColours(chromagramImage,index);
	vis->setChromagramColours(miniChromagramImage,index);
	vis->setChromagramColours(colourScaleImage,index);
	ui->chromagramLabel->setPixmap(QPixmap::fromImage(chromagramImage));
	ui->miniChromagramLabel->setPixmap(QPixmap::fromImage(miniChromagramImage));
	ui->colourScaleLabel->setPixmap(QPixmap::fromImage(colourScaleImage));
}
