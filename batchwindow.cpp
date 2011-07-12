#include "batchwindow.h"
#include "ui_batchwindow.h"

const int COL_PATH = 0;
const int COL_TAG_ARTIST = 1;
const int COL_TAG_TITLE = 2;
const int COL_TAG_GROUPING = 3;
const int COL_KEY = 4;
const int COL_KEYCODE = 5;

BatchWindow::BatchWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::BatchWindow){
	// SETUP KEYFINDER
	ab = NULL;
	sa = NULL;
	ch = NULL;
	// SETUP ASYNC SIGNALS/SLOTS
	connect(&fileDropWatcher, SIGNAL(finished()), this, SLOT(fileDropFinished()));
	connect(&analysisWatcher, SIGNAL(finished()), this, SLOT(fileFinished()));
	// SETUP UI
	ui->setupUi(this);
	allowDrops = true;
	vis = Visuals::getInstance();
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
	if(allowDrops && e->mimeData()->hasUrls()  && !e->mimeData()->urls().at(0).toLocalFile().isEmpty()){
		e->acceptProposedAction();
	}
}

void BatchWindow::dropEvent(QDropEvent *e){
	allowDrops = false;
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
			ui->tableWidget->setItem(newRow,COL_PATH,new QTableWidgetItem());
			ui->tableWidget->item(newRow,COL_PATH)->setText(filePath);
			Metadata* md = Metadata::getMetadata(filePath.toAscii().data());
			QString tag = QString::fromUtf8(md->getArtist().c_str());
			if(tag != ""){
				ui->tableWidget->setItem(newRow,COL_TAG_ARTIST,new QTableWidgetItem());
				ui->tableWidget->item(newRow,COL_TAG_ARTIST)->setText(tag);
			}
			tag = QString::fromUtf8(md->getTitle().c_str());
			if(tag != ""){
				ui->tableWidget->setItem(newRow,COL_TAG_TITLE,new QTableWidgetItem());
				ui->tableWidget->item(newRow,COL_TAG_TITLE)->setText(tag);
			}
			tag = QString::fromUtf8(md->getGrouping().c_str());
			if(tag != ""){
				ui->tableWidget->setItem(newRow,COL_TAG_GROUPING,new QTableWidgetItem());
				ui->tableWidget->item(newRow,COL_TAG_GROUPING)->setText(tag);
			}
		}
	}
}

void BatchWindow::fileDropFinished(){
	allowDrops = true;
	ui->runBatchButton->setDisabled(false);
	ui->tableWidget->resizeColumnsToContents();
	ui->tableWidget->resizeRowsToContents();
}

void BatchWindow::on_runBatchButton_clicked(){
	if(ui->tableWidget->rowCount()==0)
		return;
	ui->runBatchButton->setDisabled(true);
	allowDrops = false;
	ui->progressBar->setMaximum(ui->tableWidget->rowCount());
	currentFile = 0;
	go();
}

void BatchWindow::go(){
	if(currentFile == ui->tableWidget->rowCount()){
		cleanUpAfterRun();
		QApplication::beep();
		return;
	}else if(ui->tableWidget->item(currentFile,COL_KEY) != NULL){
		fileFinished();
	}else{
		ui->progressBar->setValue(currentFile);
		QFuture<void> future = QtConcurrent::run(this,&BatchWindow::analyseFile,currentFile);
		analysisWatcher.setFuture(future);
	}
}

void BatchWindow::analyseFile(int whichFile){
	std::string filePath = (std::string)ui->tableWidget->item(whichFile,COL_PATH)->text().toAscii();
	AudioFileDecoder* dec = AudioFileDecoder::getDecoder(filePath);
	try{
		ab = dec->decodeFile((char*)filePath.c_str());
		delete dec;
	}catch(const Exception& e){
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
		int key = hc.classify(chroma);
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
	ui->tableWidget->setItem(whichFile,COL_KEY,new QTableWidgetItem());
	ui->tableWidget->item(whichFile,COL_KEY)->setText(vis->keyNames[mostCommonKey]);
	ui->tableWidget->setItem(whichFile,COL_KEYCODE,new QTableWidgetItem());
	ui->tableWidget->item(whichFile,COL_KEYCODE)->setText(vis->keyCodes[mostCommonKey]);
}

void BatchWindow::markBroken(int whichFile){
	ui->tableWidget->setItem(whichFile,COL_KEY,new QTableWidgetItem());
	ui->tableWidget->item(whichFile,COL_KEY)->setText("Failed");
	ui->tableWidget->item(whichFile,COL_PATH)->setTextColor(qRgb(255,0,0));
	ui->tableWidget->resizeColumnsToContents();
}

void BatchWindow::fileFinished(){
	currentFile++;
	go();
}

void BatchWindow::cleanUpAfterRun(){
	allowDrops = true;
	ui->progressBar->setValue(0);
	ui->runBatchButton->setDisabled(false);
	ui->tableWidget->resizeColumnsToContents();
}

void BatchWindow::on_tableWidget_itemSelectionChanged(){
	/*
		Enabling copy to clipboard. This doesn't work exactly as expected;
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
	}else if(event->matches(QKeySequence::Refresh)){ // Refresh is a cheap way to intercept Cmd+R for "Run"
		if(ui->runBatchButton->isEnabled())
			ui->runBatchButton->click();
	}
}
