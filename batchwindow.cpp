#include "batchwindow.h"
#include "ui_batchwindow.h"

BatchWindow::BatchWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::BatchWindow){
	ui->setupUi(this);
	// SETUP KEYFINDER
	ab = NULL;
	sa = NULL;
	ch = NULL;
	vis = Visuals::getInstance();
	// SETUP ASYNC SIGNALS/SLOTS
	connect(&analysisWatcher, SIGNAL(finished()), this, SLOT(fileFinished()));
	connect(&fileDropWatcher, SIGNAL(finished()), this, SLOT(fileDropFinished()));
}

BatchWindow::~BatchWindow(){
	delete ui;
}

void BatchWindow::on_actionNew_Detail_Keyfinder_triggered(){
	DetailWindow* newWin = new DetailWindow(0);
	newWin->show();
}

void BatchWindow::on_actionNew_Batch_Keyfinder_triggered(){
	BatchWindow* newWin = new BatchWindow(0);
	newWin->show();
}

void BatchWindow::on_actionClose_Window_triggered(){
	this->close();
}

void BatchWindow::dragEnterEvent(QDragEnterEvent *e){
	// accept only local files
	if(e->mimeData()->hasUrls()  && !e->mimeData()->urls().at(0).toLocalFile().isEmpty()){
		e->acceptProposedAction();
	}
}

void BatchWindow::dropEvent(QDropEvent *e){
	ui->runBatchButton->setDisabled(true);
	QFuture<void> future = QtConcurrent::run(this,&BatchWindow::filesDropped,e->mimeData()->urls());
	fileDropWatcher.setFuture(future);
}

void BatchWindow::filesDropped(QList<QUrl> urls){
	for(int i=0; i<urls.size(); i++){
		bool addNew = true;
		QString fileName = urls[i].toLocalFile();
		for(int j=0; j<ui->tableWidget->rowCount(); j++){
			if(ui->tableWidget->item(j,0)->text() == fileName){
				addNew = false;
				break;
			}
		}
		if(addNew){
			int newRow = ui->tableWidget->rowCount(); // start at 0
			ui->tableWidget->insertRow(newRow);
			ui->tableWidget->setItem(newRow,0,new QTableWidgetItem());
			ui->tableWidget->item(newRow,0)->setText(fileName);
		}
	}
}

void BatchWindow::fileDropFinished(){
	ui->runBatchButton->setDisabled(false);
	ui->tableWidget->resizeColumnsToContents();
	ui->tableWidget->resizeRowsToContents();
}

void BatchWindow::on_runBatchButton_clicked(){
	if(ui->tableWidget->rowCount()==0)
		return;
	ui->progressBar->setMaximum(ui->tableWidget->rowCount());
	currentFile = 0;
	go();
}

void BatchWindow::go(){
	if(currentFile==ui->tableWidget->rowCount()){
		cleanUpAfterRun();
		QApplication::beep();
		return;
	}else if(ui->tableWidget->item(currentFile,1) != NULL){
		fileFinished();
	}else{
		ui->progressBar->setValue(currentFile);
		ui->runBatchButton->setDisabled(true);
		QFuture<void> future = QtConcurrent::run(this,&BatchWindow::analyseFile,currentFile);
		analysisWatcher.setFuture(future);
	}
}

void BatchWindow::analyseFile(int whichFile){
	std::string fileName = (std::string)ui->tableWidget->item(whichFile,0)->text().toAscii();
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
		markBroken(whichFile);
		return;
	}
	Monaural* mono = new basicMono();
	ab = mono->makeMono(ab);
	delete mono;
	if(prefs.getDFactor() > 1){
		Downsampler* ds = NULL;
		if(prefs.getDFactor() == 10 && ab->frameRate == 44100 && prefs.getBinFreq(-1) < 2205.0){
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
			markBroken(whichFile);
			return;
		}
	}
	sa = SpectrumAnalyserFactory::getInstance()->getSpectrumAnalyser(ab->frameRate,prefs);
	ch = sa->chromagram(ab);
	delete ab;
	ab = NULL;
	sa = NULL; // we don't delete sa; it lives on in the factory to be reused.
	ch->decomposeToTwelveBpo(prefs);
	ch->decomposeToOneOctave(prefs);
	HarteHCDF harto;
	std::vector<int> changes = harto.hcdf(ch,prefs);
	changes.push_back(ch->getHops()-1);
	HarmonicClassifier hc(prefs.getToneProfile());
	std::vector<int> trackKeys(24);
	for(int i=0; i<changes.size()-1; i++){
		std::vector<double> chroma(12);
		for(int j=changes[i]; j<changes[i+1]; j++){
			for(int k=0; k<ch->getBins(); k++){
				chroma[k] += ch->getMagnitude(j,k);
			}
		}
		int key = hc.classify(chroma,prefs);
		for(int j=changes[i]; j<changes[i+1]; j++){
			trackKeys[key]++;
		}
	}
	delete ch;
	ch = NULL;
	int mostCommonKey = -1;
	int mostCommonKeyCount = 0;
	for(int i=0; i<trackKeys.size(); i++){
		if(trackKeys[i] > mostCommonKeyCount){
			mostCommonKeyCount = trackKeys[i];
			mostCommonKey = i;
		}
	}
	ui->tableWidget->setItem(whichFile,1,new QTableWidgetItem());
	ui->tableWidget->item(whichFile,1)->setText(vis->keyNames[mostCommonKey]);
}

void BatchWindow::markBroken(int whichFile){
	ui->tableWidget->setItem(whichFile,1,new QTableWidgetItem());
	ui->tableWidget->item(whichFile,1)->setText("Failed");
	ui->tableWidget->item(whichFile,0)->setTextColor(qRgb(255,0,0));
	ui->tableWidget->resizeColumnsToContents();
}

void BatchWindow::fileFinished(){
	currentFile++;
	go();
}

void BatchWindow::cleanUpAfterRun(){
	ui->progressBar->setValue(0);
	ui->runBatchButton->setDisabled(false);
	ui->tableWidget->resizeColumnsToContents();
}

void BatchWindow::on_tableWidget_itemSelectionChanged(){
	/*
		enabling copy to clipboard: this doesn't work exactly as expected;
		it just copies from the top-left to the bottom-right selected cells.
	*/
	copyArray.clear();
	int firstRow = INT_MAX;
	int lastRow = INT_MIN;
	int firstCol = INT_MAX;
	int lastCol = INT_MIN;
	foreach(QModelIndex selectedIndex,ui->tableWidget->selectionModel()->selectedIndexes()){
		int chkRow = selectedIndex.row();
		int chkCol = selectedIndex.column();
		if(chkRow < firstRow)
			firstRow = chkRow;
		if(chkRow > lastRow)
			lastRow = chkRow;
		if(chkCol < firstCol)
			firstCol = chkCol;
		if(chkCol > lastCol)
			lastCol = chkCol;
	}
	for(int r = firstRow; r <= lastRow; r++){
		for(int c = firstCol; c <= lastCol; c++){
			QTableWidgetItem* item = ui->tableWidget->item(r,c);
			if(item != NULL)
				copyArray.append(item->text());
			copyArray.append("\t");
		}
		copyArray.append("\r\n");
	}
}

void BatchWindow::keyPressEvent(QKeyEvent* event){
	if(event->matches(QKeySequence::Copy)){
		QMimeData* mimeData = new QMimeData();
		mimeData->setData("text/plain",copyArray);
		QApplication::clipboard()->setMimeData(mimeData);
	}
}
