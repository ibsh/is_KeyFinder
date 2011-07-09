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
	connect(&fileDropWatcher, SIGNAL(finished()), this, SLOT(fileDropFinished()));
	connect(&analysisWatcher, SIGNAL(finished()), this, SLOT(fileFinished()));
}

BatchWindow::~BatchWindow(){
	fileDropWatcher.cancel();
	fileDropWatcher.waitForFinished();
	analysisWatcher.cancel();
	analysisWatcher.waitForFinished();
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
		QString filePath = urls[i].toLocalFile();
		// check if file is already in the list
		for(int j=0; j<ui->tableWidget->rowCount(); j++){
			if(ui->tableWidget->item(j,0)->text() == filePath){
				addNew = false;
				break;
			}
		}
		if(addNew){
			int newRow = ui->tableWidget->rowCount();
			ui->tableWidget->insertRow(newRow);
			ui->tableWidget->setItem(newRow,0,new QTableWidgetItem());
			ui->tableWidget->item(newRow,0)->setText(filePath);
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
	ui->runBatchButton->setDisabled(true);
	currentFile = 0;
	go();
}

void BatchWindow::go(){
	int fileCount = ui->tableWidget->rowCount();
	if(currentFile == fileCount){
		cleanUpAfterRun();
		QApplication::beep();
		return;
	}else if(ui->tableWidget->item(currentFile,1) != NULL){
		fileFinished();
	}else{
		ui->progressBar->setMaximum(fileCount); // do this in batch loop, in case more files have been added during job
		ui->progressBar->setValue(currentFile);
		QFuture<void> future = QtConcurrent::run(this,&BatchWindow::analyseFile,currentFile);
		analysisWatcher.setFuture(future);
	}
}

void BatchWindow::analyseFile(int whichFile){
	std::string filePath = (std::string)ui->tableWidget->item(whichFile,0)->text().toAscii();
	AudioFileDecoder* dec = AudioFileDecoder::getDecoder(filePath);
	try{
		ab = dec->decodeFile((char*)filePath.c_str());
		delete dec;
	}catch(const Exception& e){
		std::cerr << e.getMessage() << std::endl;
		delete ab;
		delete dec;
		ab = NULL;
		markBroken(whichFile);
		return;
	}
	Monaural* mono = new Monaural();
	ab = mono->makeMono(ab);
	delete mono;
	if(prefs.getDFactor() > 1){
		Downsampler* ds = Downsampler::getDownsampler(prefs.getDFactor(),ab->getFrameRate(),prefs.getBinFreq(-1));
		try{
			ab = ds->downsample(ab,prefs.getDFactor());
			delete ds;
		}catch(const Exception& e){
			std::cerr << e.getMessage() << std::endl;
			delete ab;
			delete ds;
			ab = NULL;
			markBroken(whichFile);
			return;
		}
	}
	sa = SpectrumAnalyserFactory::getInstance()->getSpectrumAnalyser(ab->getFrameRate(),prefs);
	ch = sa->chromagram(ab);
	delete ab;
	ab = NULL;
	sa = NULL; // don't delete sa; it lives on in the factory for reuse.
	ch->decomposeToTwelveBpo(prefs);
	ch->decomposeToOneOctave(prefs);
	Hcdf harto;
	std::vector<int> changes = harto.hcdf(ch,prefs);
	changes.push_back(ch->getHops()-1);
	KeyClassifier hc(prefs.getToneProfile());
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
	ui->tableWidget->setItem(whichFile,2,new QTableWidgetItem());
	ui->tableWidget->item(whichFile,2)->setText(vis->keyCodes[mostCommonKey]);
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
