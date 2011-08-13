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

#include "batchwindow.h"
#include "ui_batchwindow.h"

const int COL_PATH = 0;
const int COL_TAG_ARTIST = 1;
const int COL_TAG_TITLE = 2;
const int COL_TAG_GROUPING = 3;
const int COL_KEY = 4;
const int COL_KEYCODE = 5;

BatchWindow::BatchWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::BatchWindow){
	// ASYNC
	connect(&fileDropWatcher, SIGNAL(finished()), this, SLOT(fileDropFinished()));
	// SETUP UI
	ui->setupUi(this);
	allowDrops = true;
	vis = Visuals::getInstance();
	// HELP LABEL
	initialHelp = new QLabel("Drag audio files here", ui->tableWidget);
	QFont font;
	font.setPointSize(20);
	font.setBold(true);
	QPalette palette = initialHelp->palette();
	palette.setColor(initialHelp->foregroundRole(), Qt::gray);
	initialHelp->setPalette(palette);
	initialHelp->setFont(font);
	// see if you can lose these magic numbers
	initialHelp->setGeometry(
			(676 - initialHelp->sizeHint().width()) / 2,
			(312 - initialHelp->sizeHint().height()) / 2,
			initialHelp->sizeHint().width(),
			initialHelp->sizeHint().height()
	);
	initialHelp->show();
	// SETUP TABLE WIDGET CONTEXT MENU
	QAction* copyAction = new QAction(tr("Copy"),this);
	copyAction->setShortcut(QKeySequence::Copy);
	connect(copyAction, SIGNAL(triggered()), this, SLOT(copySelectedFromTableWidget()));
	ui->tableWidget->addAction(copyAction);
	QAction* writeToGroupingAction = new QAction(tr("Write key to Grouping tag"),this);
	writeToGroupingAction->setShortcut(QKeySequence("Ctrl+T"));
	connect(writeToGroupingAction, SIGNAL(triggered()), this, SLOT(writeDetectedToGrouping()));
	ui->tableWidget->addAction(writeToGroupingAction);
	QAction* runDetailedAction = new QAction(tr("Run detailed analysis"),this);
	runDetailedAction->setShortcut(QKeySequence("Ctrl+D"));
	connect(runDetailedAction, SIGNAL(triggered()), this, SLOT(runDetailedAnalysis()));
	ui->tableWidget->addAction(runDetailedAction);
}

BatchWindow::~BatchWindow(){
	delete ui;
}

void BatchWindow::dragEnterEvent(QDragEnterEvent *e){
	// accept only local files
	if(allowDrops && e->mimeData()->hasUrls() && !e->mimeData()->urls().at(0).toLocalFile().isEmpty()){
		e->acceptProposedAction();
	}
}

void BatchWindow::dropEvent(QDropEvent *e){
	allowDrops = false;
	ui->runBatchButton->setDisabled(true);
	QFuture<void> future = QtConcurrent::run(this,&BatchWindow::filesDropped,e->mimeData()->urls());
	fileDropWatcher.setFuture(future);
}

QStringList BatchWindow::getDirectoryContents(QDir dir){
	// regex to exclude all hidden files and directory recursion
	QStringList contents = dir.entryList().filter(QRegExp("[^.]"));
	QStringList results;
	for(int i = 0; i<(signed)contents.size(); i++){
		if(QFileInfo(contents[i]).isDir())
			results += getDirectoryContents(QDir(contents[i]));
		else
			results.push_back(dir.path() + "/" + contents[i]);
	}
	return results;
}

void BatchWindow::filesDropped(QList<QUrl>& urls){
	for(int i=0; i<urls.size(); i++){
		QString filePath = urls[i].toLocalFile();
		// check if url is a directory; if so, get contents rather than adding
		if(QFileInfo(filePath).isDir()){
			QStringList contents = getDirectoryContents(QDir(filePath));
			for(int j=0; j<contents.size(); j++)
				urls.push_back(QUrl(contents[j]));
			continue;
		}
		// check if path is already in the list
		bool isNew = true;
		for(int j=0; j<ui->tableWidget->rowCount(); j++){
			if(ui->tableWidget->item(j,COL_PATH)->text() == filePath){
				isNew = false;
				break;
			}
		}
		if(isNew)
			addNewRow(filePath);
	}
}

void BatchWindow::addNewRow(QString filePath){
	if(initialHelp != NULL){
		delete initialHelp;
		initialHelp = NULL;
	}
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

void BatchWindow::fileDropFinished(){
	allowDrops = true;
	ui->runBatchButton->setDisabled(false);
	ui->tableWidget->resizeColumnsToContents();
	ui->tableWidget->resizeRowsToContents();
}

void BatchWindow::on_runBatchButton_clicked(){
	// get a new preferences object in case they've changed since the last run.
	prefs = Preferences();
	if(ui->tableWidget->rowCount()==0)
		return;
	ui->runBatchButton->setDisabled(true);
	ui->tableWidget->setContextMenuPolicy(Qt::NoContextMenu); // so that no tags can be written while busy
	allowDrops = false;
	ui->progressBar->setMaximum(ui->tableWidget->rowCount());
	currentFile = 0;
	processCurrentFile();
}

void BatchWindow::processCurrentFile(){
	if(currentFile == ui->tableWidget->rowCount()){
		cleanUpAfterRun();
		QApplication::beep();
		return;
	}else if(ui->tableWidget->item(currentFile,COL_KEY) != NULL){
		// don't reprocess files
		currentFile++;
		processCurrentFile();
	}else{
		ui->progressBar->setValue(currentFile);
		// now proceed
		modelThread = new KeyFinderWorkerThread(ui->tableWidget->item(currentFile,COL_PATH)->text(),prefs);
		connect(modelThread,SIGNAL(failed(QString)),this,SLOT(fileFailed()),Qt::QueuedConnection);
		connect(modelThread,SIGNAL(producedGlobalKeyEstimate(int)),this,SLOT(fileFinished(int)),Qt::QueuedConnection);
		modelThread->start();
	}
}

void BatchWindow::cleanUpAfterRun(){
	allowDrops = true;
	ui->progressBar->setValue(0);
	ui->runBatchButton->setDisabled(false);
	ui->tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
	ui->tableWidget->resizeColumnsToContents();
}

void BatchWindow::fileFinished(int key){
	ui->tableWidget->setItem(currentFile,COL_KEY,new QTableWidgetItem());
	ui->tableWidget->item(currentFile,COL_KEY)->setText(vis->keyNames[key]);
	ui->tableWidget->setItem(currentFile,COL_KEYCODE,new QTableWidgetItem());
	ui->tableWidget->item(currentFile,COL_KEYCODE)->setText(vis->keyCodes[key]);
	currentFile++;
	processCurrentFile();
}

void BatchWindow::fileFailed(){
	ui->tableWidget->setItem(currentFile,COL_KEY,new QTableWidgetItem());
	ui->tableWidget->item(currentFile,COL_KEY)->setText("Failed");
	ui->tableWidget->item(currentFile,COL_KEY)->setTextColor(qRgb(255,0,0));
	ui->tableWidget->item(currentFile,COL_PATH)->setTextColor(qRgb(255,0,0));
	currentFile++;
	processCurrentFile();
}

void BatchWindow::copySelectedFromTableWidget(){
	copyArray.clear();
	int firstRow = INT_MAX;
	int lastRow = 0;
	int firstCol = INT_MAX;
	int lastCol = 0;
	foreach(QModelIndex selectedIndex,ui->tableWidget->selectionModel()->selectedIndexes()){
		int chkRow = selectedIndex.row();
		int chkCol = selectedIndex.column();
		if(chkRow < firstRow) firstRow = chkRow;
		if(chkRow > lastRow) lastRow = chkRow;
		if(chkCol < firstCol) firstCol = chkCol;
		if(chkCol > lastCol) lastCol = chkCol;
	}
	for(int r = firstRow; r <= lastRow; r++){
		for(int c = firstCol; c <= lastCol; c++){
			QTableWidgetItem* item = ui->tableWidget->item(r,c);
			if(item != NULL && item->isSelected())
				copyArray.append(item->text());
			copyArray.append("\t");
		}
		copyArray.append("\r\n");
	}
	QMimeData* mimeData = new QMimeData();
	mimeData->setData("text/plain",copyArray);
	QApplication::clipboard()->setMimeData(mimeData);
}

void BatchWindow::writeDetectedToGrouping(){
	int firstRow = INT_MAX;
	int lastRow = 0;
	foreach(QModelIndex selectedIndex,ui->tableWidget->selectionModel()->selectedIndexes()){
		int chkRow = selectedIndex.row();
		if(chkRow < firstRow) firstRow = chkRow;
		if(chkRow > lastRow) lastRow = chkRow;
	}
	for(int r = firstRow; r <= lastRow; r++){
		QTableWidgetItem* item = ui->tableWidget->item(r,COL_KEY); // only write if there's a detected key
		if(item != NULL && item->text() != "Failed"){
			Metadata* md = Metadata::getMetadata(ui->tableWidget->item(r,COL_PATH)->text().toAscii().data());
			md->setGrouping((ui->tableWidget->item(r,COL_KEYCODE)->text() + " " + ui->tableWidget->item(r,COL_KEY)->text()).toAscii().data());
		}
	}
	QMessageBox msg;
	QString msgText = "Tags written to ";
	msgText += QString("%1").arg(lastRow-firstRow+1);
	msgText += " files";
	msg.setText(msgText);
	msg.exec();
}

void BatchWindow::runDetailedAnalysis(){
	int firstRow = INT_MAX;
	int lastRow = 0;
	foreach(QModelIndex selectedIndex,ui->tableWidget->selectionModel()->selectedIndexes()){
		int chkRow = selectedIndex.row();
		if(chkRow < firstRow) firstRow = chkRow;
		if(chkRow > lastRow) lastRow = chkRow;
	}
	if(firstRow != lastRow){
		QMessageBox msg;
		QString msgText = "Please select only a single row for detailed analysis";
		msg.setText(msgText);
		msg.exec();
		return;
	}
	DetailWindow* newWin = new DetailWindow(0,ui->tableWidget->item(firstRow,COL_PATH)->text());
	newWin->show();
}
