#include "detailwindow.h"
#include "ui_detailwindow.h"

DetailWindow::DetailWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::DetailWindow){
	// SETUP VISUALS
	ui->setupUi(this);
	ui->progressBar->setVisible(false);
	QImage blankImage(1,1,QImage::Format_Indexed8);
	ui->chromagramLabel->setPixmap(QPixmap::fromImage(blankImage));
	ui->segmentLabel->setPixmap(QPixmap::fromImage(blankImage));
	keyNames = std::vector<QString>(0);
	keyNames.push_back("A");		keyNames.push_back("Am");
	keyNames.push_back("Bb");		keyNames.push_back("Bbm");
	keyNames.push_back("B");		keyNames.push_back("Bm");
	keyNames.push_back("C");		keyNames.push_back("Cm");
	keyNames.push_back("Db");		keyNames.push_back("Dbm");
	keyNames.push_back("D");		keyNames.push_back("Dm");
	keyNames.push_back("Eb");		keyNames.push_back("Ebm");
	keyNames.push_back("E");		keyNames.push_back("Em");
	keyNames.push_back("F");		keyNames.push_back("Fm");
	keyNames.push_back("Gb");		keyNames.push_back("Gbm");
	keyNames.push_back("G");		keyNames.push_back("Gm");
	keyNames.push_back("Ab");		keyNames.push_back("Abm");
	// SETUP KEYFINDER
	fileLoaded = false;
	ab = NULL;
	sa = NULL;
	ch = NULL;
	// SETUP ASYNC SIGNALS/SLOTS (do it here so they don't repeat if a decode fails).
	connect(&decodeWatcher, SIGNAL(finished()), this, SLOT(decoded()));
	connect(&monoWatcher, SIGNAL(finished()), this, SLOT(madeMono()));
	connect(&dsWatcher, SIGNAL(finished()), this, SLOT(downSampled()));
	connect(&saInitWatcher, SIGNAL(finished()), this, SLOT(saInitialised()));
	connect(&saWatcher, SIGNAL(finished()), this, SLOT(saAnalysed()));
	connect(&haWatcher, SIGNAL(finished()), this, SLOT(haFinished()));
}

void DetailWindow::dropEvent(QDropEvent *e){
	fileLoaded = true;
	fileName = (std::string)e->mimeData()->urls().at(0).toLocalFile().toAscii();
	go();
}

void DetailWindow::go(){
	say("Decoding audio... ");
	ui->progressBar->setValue(1);
	ui->progressBar->setVisible(true);
	ui->saCombo->setDisabled(true);
	ui->twCombo->setDisabled(true);
	ui->tpCombo->setDisabled(true);
	ui->runButton->setDisabled(true);
	// Asynchronous, threaded computation
	QFuture<void> future = QtConcurrent::run(this,&DetailWindow::decode);
	decodeWatcher.setFuture(future);
	//say(QString::fromUtf8(e.message.c_str())); // std::string -> QString
}

void DetailWindow::cleanUpAfterRun(){
	ui->progressBar->setValue(0);
	ui->progressBar->setVisible(false);
	ui->saCombo->setDisabled(false);
	ui->twCombo->setDisabled(false);
	ui->tpCombo->setDisabled(false);
	ui->runButton->setDisabled(false);
}

void DetailWindow::decode(){
	AudioFileDecoder* dec = NULL;
	std::string fileExt  = fileName.substr(fileName.rfind(".")+1);
	// choose the default for this file extension
	if(fileExt == "wav" || fileExt == "aif" || fileExt == "aiff" || fileExt == "flac"){
		dec = new LibSndFileDecoder();
	}else{
		dec = new LibAvDecoder();
	}
	try{
		ab = dec->decodeFile((char*)fileName.c_str());
		delete dec;
	}catch(FatalException e){
		delete ab;
		delete dec;
		ab = NULL;
	}
}

void DetailWindow::decoded(){
	if(ab==NULL){
		say("Decode failed. Try another file.");
		fileLoaded = false;
		cleanUpAfterRun();
		return;
	}
	say("Decoded, making mono...");
	ui->progressBar->setValue(2);
	QFuture<void> future = QtConcurrent::run(this,&DetailWindow::makeMono);
	monoWatcher.setFuture(future);
}

void DetailWindow::makeMono(){
	Monaural* mono = new basicMono();
	ab = mono->makeMono(ab);
	delete mono;
}

void DetailWindow::madeMono(){
	if(prefs.getDFactor() < 2){
		say("Made mono. Initialising spectrum analyser...");
		ui->progressBar->setValue(4);
		QFuture<void> future = QtConcurrent::run(this,&DetailWindow::saInitialise);
		saInitWatcher.setFuture(future);
	}else{
		say("Made mono. Downsampling... ");
		ui->progressBar->setValue(3);
		QFuture<void> future = QtConcurrent::run(this,&DetailWindow::downSample);
		dsWatcher.setFuture(future);
	}
}

void DetailWindow::downSample(){
	Downsampler* ds = NULL;
	// discard downsampler no longer useable here. But it was crap anyway.
	if(prefs.getDFactor() == 10 && ab->frameRate == 44100){
		ds = new IbDownsampler();
	}else{
		ds = new SecretRabbitDownsampler();
	}
	try{
		ab = ds->downsample(ab,prefs.getDFactor());
		delete ds;
	}catch(FatalException e){
		delete ab;
		delete ds;
		ab = NULL;
	}
}

void DetailWindow::downSampled(){
	if(ab==NULL){
		say("Downsample failed. Try again.");
		fileLoaded = false;
		cleanUpAfterRun();
		return;
	}
	say("Downsampled. Initialising spectrum analyser...");
	ui->progressBar->setValue(4);
	QFuture<void> future = QtConcurrent::run(this,&DetailWindow::saInitialise);
	saInitWatcher.setFuture(future);
}

void DetailWindow::saInitialise(){
	sa = NULL;
	if(prefs.getSpectrumAnalyser() == 'g'){
		sa = new GoertzelAnalyser(ab->frameRate,prefs);
	}else{
		sa = new FftwAnalyser(ab->frameRate,prefs);
	}
}

void DetailWindow::saInitialised(){
	say("Running spectrum analysis...");
	ui->progressBar->setValue(5);
	QFuture<void> future = QtConcurrent::run(this,&DetailWindow::saAnalyse);
	saWatcher.setFuture(future);
}

void DetailWindow::saAnalyse(){
	ch = sa->chromagram(ab);
}

void DetailWindow::saAnalysed(){
	// Paint 72-bin chroma to screen in 64 colours
	QImage chromagramImage(ch->getHops(),72,QImage::Format_Indexed8);
	setChromagramColours(chromagramImage);
	// get max to normalise
	float norm = -1;
	std::vector<std::vector<float> > chromaBuilder(ch->getHops(),std::vector<float>(ch->getBins()));
	for(int h=0; h<ch->getHops(); h++){
		for(int b=0; b<ch->getBins(); b++){
			float comp = ch->getMagnitude(h,b);
			chromaBuilder[h][b] = comp;
			if(comp>norm) norm = comp;
		}
	}
	// set pixels
	for(int h=0; h<ch->getHops(); h++){
		for(int b=0; b<ch->getBins(); b++){
			int pixVal =((chromaBuilder[h][b] / norm) * 64) - 1;
			if(pixVal==-1) pixVal = 0;
			chromagramImage.setPixel(h, 71-b, pixVal);
		}
	}
	// show
	ui->chromagramLabel->setPixmap(QPixmap::fromImage(chromagramImage));
	say("Running harmonic analyses...");
	ui->progressBar->setValue(6);
	QFuture<void> future = QtConcurrent::run(this,&DetailWindow::harmonicAnalysis);
	haWatcher.setFuture(future);
}

void DetailWindow::harmonicAnalysis(){
	ch->decomposeToOneOctave();
	HarteHCDF harto;
	std::vector<int> changes = harto.hcdf(ch);
	changes.push_back(ch->getHops()-1); // add sentinel to back end
	HarmonicClassifier hc(prefs.getToneProfile());
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
	// Paint keychanges to screen
	QImage keyImage(ch->getHops(),1,QImage::Format_Indexed8);
	setKeyColours(keyImage);
	for(int h=0; h<keys.size(); h++){
		keyImage.setPixel(h, 0, keys[h]);
	}
	ui->segmentLabel->setPixmap(QPixmap::fromImage(keyImage));
	// finale
	say(QString::fromUtf8(fileName.substr(fileName.rfind("/")+1).c_str()) + " complete.");
	cleanUpAfterRun();
}

void DetailWindow::dragEnterEvent(QDragEnterEvent *e){
	// accept only single, local files, and reject if there's already a file in the window
	if(
		 !fileLoaded
		 && e->mimeData()->hasUrls()
		 && e->mimeData()->urls().size() == 1
		 && !e->mimeData()->urls().at(0).toLocalFile().isEmpty()
	){
		e->acceptProposedAction();
	}
}

DetailWindow::~DetailWindow(){
	delete ui;
}

void DetailWindow::setChromagramColours(QImage& image, int which){
	if(which==0){
		image.setColor(0,qRgb(0,1,3));
		image.setColor(1,qRgb(1,4,8));
		image.setColor(2,qRgb(1,7,12));
		image.setColor(3,qRgb(2,9,17));
		image.setColor(4,qRgb(2,12,24));
		image.setColor(5,qRgb(3,13,24));
		image.setColor(6,qRgb(4,15,27));
		image.setColor(7,qRgb(5,16,30));
		image.setColor(8,qRgb(6,18,33));
		image.setColor(9,qRgb(7,20,36));
		image.setColor(10,qRgb(8,22,39));
		image.setColor(11,qRgb(9,23,42));
		image.setColor(12,qRgb(10,25,45));
		image.setColor(13,qRgb(11,26,48));
		image.setColor(14,qRgb(14,29,50));
		image.setColor(15,qRgb(18,31,52));
		image.setColor(16,qRgb(21,33,54));
		image.setColor(17,qRgb(24,36,55));
		image.setColor(18,qRgb(27,38,57));
		image.setColor(19,qRgb(32,40,56));
		image.setColor(20,qRgb(38,43,55));
		image.setColor(21,qRgb(43,45,54));
		image.setColor(22,qRgb(49,48,53));
		image.setColor(23,qRgb(56,51,52));
		image.setColor(24,qRgb(65,53,50));
		image.setColor(25,qRgb(74,56,47));
		image.setColor(26,qRgb(84,58,45));
		image.setColor(27,qRgb(93,61,43));
		image.setColor(28,qRgb(103,64,40));
		image.setColor(29,qRgb(113,67,39));
		image.setColor(30,qRgb(123,69,37));
		image.setColor(31,qRgb(133,71,36));
		image.setColor(32,qRgb(142,74,34));
		image.setColor(33,qRgb(152,76,32));
		image.setColor(34,qRgb(160,79,32));
		image.setColor(35,qRgb(170,82,32));
		image.setColor(36,qRgb(179,85,32));
		image.setColor(37,qRgb(188,88,32));
		image.setColor(38,qRgb(196,91,32));
		image.setColor(39,qRgb(203,96,33));
		image.setColor(40,qRgb(210,101,34));
		image.setColor(41,qRgb(216,106,35));
		image.setColor(42,qRgb(223,112,36));
		image.setColor(43,qRgb(230,118,38));
		image.setColor(44,qRgb(236,123,39));
		image.setColor(45,qRgb(239,134,44));
		image.setColor(46,qRgb(241,147,49));
		image.setColor(47,qRgb(242,160,55));
		image.setColor(48,qRgb(244,173,60));
		image.setColor(49,qRgb(246,186,66));
		image.setColor(50,qRgb(248,200,72));
		image.setColor(51,qRgb(249,209,82));
		image.setColor(52,qRgb(249,218,92));
		image.setColor(53,qRgb(249,226,102));
		image.setColor(54,qRgb(250,234,113));
		image.setColor(55,qRgb(250,243,122));
		image.setColor(56,qRgb(251,251,132));
		image.setColor(57,qRgb(251,255,147));
		image.setColor(58,qRgb(252,255,163));
		image.setColor(59,qRgb(253,255,178));
		image.setColor(60,qRgb(253,255,194));
		image.setColor(61,qRgb(254,255,210));
		image.setColor(62,qRgb(254,255,225));
		image.setColor(63,qRgb(255,255,241));
	}
}

void DetailWindow::setKeyColours(QImage& image, int which){

	if(which==0){
		image.setColor(0,qRgb(127,255,0));
		image.setColor(1,qRgb(63,127,0));
		image.setColor(2,qRgb(255,0,255));
		image.setColor(3,qRgb(127,0,127));
		image.setColor(4,qRgb(0,255,127));
		image.setColor(5,qRgb(0,127,63));
		image.setColor(6,qRgb(255,0,0));
		image.setColor(7,qRgb(127,0,0));
		image.setColor(8,qRgb(0,127,255));
		image.setColor(9,qRgb(0,63,127));
		image.setColor(10,qRgb(255,255,0));
		image.setColor(11,qRgb(127,127,0));
		image.setColor(12,qRgb(127,0,255));
		image.setColor(13,qRgb(63,0,127));
		image.setColor(14,qRgb(0,255,0));
		image.setColor(15,qRgb(0,127,0));
		image.setColor(16,qRgb(255,0,127));
		image.setColor(17,qRgb(127,0,63));
		image.setColor(18,qRgb(0,255,255));
		image.setColor(19,qRgb(0,127,127));
		image.setColor(20,qRgb(255,127,0));
		image.setColor(21,qRgb(127,63,0));
		image.setColor(22,qRgb(0,0,255));
		image.setColor(23,qRgb(0,0,127));
	}
}

void DetailWindow::say(const QString& s){
	ui->statusLabel->setText(s);
}

void DetailWindow::on_actionNew_Detail_Keyfinder_triggered(){
	DetailWindow* newWin = new DetailWindow(0);
	newWin->show();
}

void DetailWindow::on_actionClose_Window_triggered(){
	this->close();
}

void DetailWindow::on_saCombo_currentIndexChanged(int index){
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

void DetailWindow::on_twCombo_currentIndexChanged(int index){
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

void DetailWindow::on_tpCombo_currentIndexChanged(int index){
	prefs.setToneProfile(index);
}

void DetailWindow::on_runButton_clicked(){
	if(fileLoaded) go();
}
