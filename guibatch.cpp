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

#include "guibatch.h"
#include "ui_batchwindow.h"

const int COL_STATUS = 0;
const int COL_FILEPATH = 1;
const int COL_FILENAME = 2;
const int COL_TAG_ARTIST = 3;
const int COL_TAG_TITLE = 4;
const int COL_TAG_COMMENT = 5;
const int COL_TAG_GROUPING = 6;
const int COL_TAG_KEY = 7;
const int COL_KEY = 8;

// Statuses >= 0 are key codes
const QString STATUS_NEW = "-1";
const QString STATUS_TAGSREAD = "-2";
const QString STATUS_SKIPPED = "-3";
const QString STATUS_FAILED = "-4";

BatchWindow::BatchWindow(MainMenuHandler* handler, QWidget* parent) : QMainWindow(parent), ui(new Ui::BatchWindow){
  // ASYNC
  connect(&addFileWatcher, SIGNAL(finished()), this, SLOT(fileDropFinished()));

  connect(&analysisWatcher, SIGNAL(finished()), this, SLOT(analysisFinished()));
  connect(&analysisWatcher, SIGNAL(canceled()), this, SLOT(analysisCancelled()));
  connect(&analysisWatcher, SIGNAL(progressRangeChanged(int, int)), this, SLOT(progressRangeChanged(int,int)));
  connect(&analysisWatcher, SIGNAL(progressValueChanged(int)), this, SLOT(progressValueChanged(int)));
  connect(&analysisWatcher, SIGNAL(resultReadyAt(int)), this, SLOT(resultReadyAt(int)));

  // SETUP UI
  ui->setupUi(this);
  allowDrops = true;
  menuHandler = handler;
  ui->tableWidget->setColumnHidden(COL_STATUS,true);
  ui->tableWidget->setColumnHidden(COL_FILEPATH,true);
  keyRow = QBrush(QColor(191,255,191));
  keyAltRow = QBrush(QColor(127,234,127));
  textDefault = QBrush(QColor(0,0,0));
  textSuccess = QBrush(QColor(0,128,0));
  textError = QBrush(QColor(191,0,0));

  //relative sizing on Mac only
#ifdef Q_OS_MAC
  QFont smallerFont;
  smallerFont.setPointSize(smallerFont.pointSize() - 2);
  ui->tableWidget->setFont(smallerFont);
#endif

  // HELP LABEL
  initialHelpLabel = new QLabel("Drag audio files here", ui->tableWidget);
  QFont font;
  font.setPointSize(20);
  font.setBold(true);
  QPalette palette = initialHelpLabel->palette();
  palette.setColor(initialHelpLabel->foregroundRole(), Qt::gray);
  initialHelpLabel->setPalette(palette);
  initialHelpLabel->setFont(font);
  // can't seem to derive these magic numbers from any useful size hints
  initialHelpLabel->setGeometry(
        (676 - initialHelpLabel->sizeHint().width()) / 2,
        (312 - initialHelpLabel->sizeHint().height()) / 2,
        initialHelpLabel->sizeHint().width(),
        initialHelpLabel->sizeHint().height()
        );
  initialHelpLabel->show();

  // SETUP TABLE WIDGET CONTEXT MENU
  QAction* copyAction = new QAction(tr("Copy"),this);
  copyAction->setShortcut(QKeySequence::Copy);
  connect(copyAction, SIGNAL(triggered()), this, SLOT(copySelectedFromTableWidget()));
  ui->tableWidget->addAction(copyAction);
  QAction* writeToTagsAction = new QAction(tr("Write key to tags"),this);
  writeToTagsAction->setShortcut(QKeySequence("Ctrl+T"));
  connect(writeToTagsAction, SIGNAL(triggered()), this, SLOT(writeDetectedToTags()));
  ui->tableWidget->addAction(writeToTagsAction);
  QAction* runDetailedAction = new QAction(tr("Run detailed analysis"),this);
  runDetailedAction->setShortcut(QKeySequence("Ctrl+D"));
  connect(runDetailedAction, SIGNAL(triggered()), this, SLOT(runDetailedAnalysis()));
  ui->tableWidget->addAction(runDetailedAction);
  QAction* clearDetectedAction = new QAction(tr("Clear detected keys"),this);
  connect(clearDetectedAction, SIGNAL(triggered()), this, SLOT(clearDetected()));
  ui->tableWidget->addAction(clearDetectedAction);
}

BatchWindow::~BatchWindow(){
  addFileWatcher.cancel();
  analysisWatcher.cancel();
  addFileWatcher.waitForFinished();
  analysisWatcher.waitForFinished();
  delete ui;
}


void BatchWindow::dragEnterEvent(QDragEnterEvent *e){
  if(allowDrops && e->mimeData()->hasUrls()){
    e->acceptProposedAction();
  }
}

void BatchWindow::dropEvent(QDropEvent *e){
  receiveUrls(e->mimeData()->urls());
}

bool BatchWindow::receiveUrls(const QList<QUrl>& urls){
  if(analysisWatcher.isRunning())
    return false;
  droppedFiles << urls;
  if(!addFileWatcher.isRunning()){
    allowDrops = false;
    ui->runBatchButton->setEnabled(false);
    ui->statusLabel->setText("Loading files...");
    QFuture<void> future = QtConcurrent::run(this,&BatchWindow::addDroppedFiles);
    addFileWatcher.setFuture(future);
  }
  return true;
}

void BatchWindow::addDroppedFiles(){
  while(!droppedFiles.isEmpty()){
    QString filePath = droppedFiles.first().toLocalFile();
    // check URL resolves to local file
    if(filePath.isEmpty())
      continue;
    QFileInfo fileInfo(filePath);
    droppedFiles.removeFirst();
    // check if url is a directory; if so, get contents rather than adding
    if(fileInfo.isDir()){
      droppedFiles << getDirectoryContents(QDir(filePath));
      continue;
    }
    // check if url is a symlink (.isSymLink doesn't seem to work on Lion)
    if(fileInfo.isSymLink() || fileInfo.symLinkTarget() != ""){
      droppedFiles.push_back(QUrl(fileInfo.symLinkTarget()));
      continue;
    }
    QString fileExt = filePath.right(3);
    if(fileExt == "m3u"){
      droppedFiles << loadPlaylistM3u(filePath);
      continue;
    }else if(fileExt == "xml"){
      droppedFiles << loadPlaylistXml(filePath);
      continue;
    }
    // check if path is already in the list
    bool isNew = true;
    for(int j=0; j<ui->tableWidget->rowCount(); j++){
      if(ui->tableWidget->item(j,COL_FILEPATH)->text() == filePath){
        isNew = false;
        break;
      }
    }
    if(isNew){
      addNewRow(filePath);
    }
  }
  this->setWindowTitle("KeyFinder - Batch Analysis - " + QString::number(ui->tableWidget->rowCount()) + " files");
  getMetadata();
}

QList<QUrl> BatchWindow::getDirectoryContents(QDir dir) const{
  QFileInfoList contents = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst);
  QList<QUrl> results;
  for(int i = 0; i<(signed)contents.size(); i++){
    if(QFileInfo(contents[i].filePath()).isDir()){
      results << getDirectoryContents(QDir(contents[i].filePath()));
    }else{
      results.push_back(QUrl::fromLocalFile(contents[i].filePath()));
    }
  }
  return results;
}

QList<QUrl> BatchWindow::loadPlaylistM3u(QString m3uUrl) const{
  // Here be ugly.
  QList<QUrl> results;
  QFile m3uFile(m3uUrl);
  if(!m3uFile.open(QIODevice::ReadOnly))
    return results;
  QTextStream m3uTextStream(&m3uFile);
  QString m3uChar;
  QString m3uLine;
  // M3U files break with ch10/13, and comment with ch35.
  // QTextStream.readLine doesn't work, so we do it a char at a time
  while(!(m3uChar = m3uTextStream.read(1)).isNull()){
    int chVal = int(m3uChar[0].toAscii());
    if(chVal == 13 || chVal == 10){
      //std::cerr << "Line (length " << m3uLine.length() << "): " << m3uLine.toLocal8Bit().data() << std::endl;
      if(m3uLine.length() > 0 && int(m3uLine[0].toAscii()) != 35){
        results.push_back(QUrl(m3uLine));
      }
      m3uLine = "";
    }else{
      m3uLine += m3uChar;
    }
  }
  return results;
}

QList<QUrl> BatchWindow::loadPlaylistXml(QString xmlFileUrl) const{
  // Here be more ugly.
  QStringList resultStrings;
  QList<QUrl> results;
  QFile xmlFile(xmlFileUrl);
  if (!xmlFile.open(QIODevice::ReadOnly))
    return results;

  // I want the text contents of the <string> node following a sibling <key> node of value "Location"
  // but for the moment the last string node seems to do it...
  QXmlQuery xmlQuery;
  xmlQuery.bindVariable("inputDocument", &xmlFile);
  xmlQuery.setQuery("doc($inputDocument)/plist/dict/dict/dict/string[last()]/string(text())");
  if (!xmlQuery.isValid())
    return results;

  xmlQuery.evaluateTo(&resultStrings);
  xmlFile.close();

  // replace iTunes' localhost addressing.
  for(int i=0; i<(signed)resultStrings.size(); i++)
    results.push_back(QUrl(resultStrings[i].replace(QString("//localhost"),QString("")).toLocal8Bit()));
  return results;
}

void BatchWindow::addNewRow(QString fileUrl){
  if(initialHelpLabel != NULL){
    delete initialHelpLabel;
    initialHelpLabel = NULL;
  }
  int newRow = ui->tableWidget->rowCount();
  ui->tableWidget->insertRow(newRow);
  ui->tableWidget->setItem(newRow,COL_STATUS,new QTableWidgetItem());
  ui->tableWidget->item(newRow,COL_STATUS)->setText(STATUS_NEW);
  ui->tableWidget->setItem(newRow,COL_FILEPATH,new QTableWidgetItem());
  ui->tableWidget->item(newRow,COL_FILEPATH)->setText(fileUrl);
  ui->tableWidget->setItem(newRow,COL_FILENAME,new QTableWidgetItem());
  ui->tableWidget->item(newRow,COL_FILENAME)->setText(fileUrl.mid(fileUrl.lastIndexOf(QDir::separator()) + 1));
  ui->tableWidget->setItem(newRow,COL_KEY,new QTableWidgetItem());
  if(newRow % 2 == 0){
    ui->tableWidget->item(newRow,COL_KEY)->setBackground(keyRow);
  }else{
    ui->tableWidget->item(newRow,COL_KEY)->setBackground(keyAltRow);
  }
}

void BatchWindow::getMetadata(){

  ui->progressBar->setMaximum(ui->tableWidget->rowCount());
  ui->statusLabel->setText("Reading tags...");

  for(int i=0; i<(signed)ui->tableWidget->rowCount(); i++){

    ui->progressBar->setValue(i);

    if(ui->tableWidget->item(i,COL_STATUS)->text() != STATUS_NEW)
      continue;
    ui->tableWidget->item(i,COL_STATUS)->setText(STATUS_TAGSREAD);

    TagLibMetadata md(ui->tableWidget->item(i,COL_FILEPATH)->text());

    QString tag = md.getArtist();
    if(tag != ""){
      ui->tableWidget->setItem(i,COL_TAG_ARTIST,new QTableWidgetItem());
      ui->tableWidget->item(i,COL_TAG_ARTIST)->setText(tag);
    }

    tag = md.getTitle();
    if(tag != ""){
      ui->tableWidget->setItem(i,COL_TAG_TITLE,new QTableWidgetItem());
      ui->tableWidget->item(i,COL_TAG_TITLE)->setText(tag);
    }

    tag = md.getComment();
    if(tag != ""){
      ui->tableWidget->setItem(i,COL_TAG_COMMENT,new QTableWidgetItem());
      ui->tableWidget->item(i,COL_TAG_COMMENT)->setText(tag);
    }

    tag = md.getGrouping();
    if(tag != ""){
      ui->tableWidget->setItem(i,COL_TAG_GROUPING,new QTableWidgetItem());
      ui->tableWidget->item(i,COL_TAG_GROUPING)->setText(tag);
    }

    tag = md.getKey();
    if(tag != ""){
      ui->tableWidget->setItem(i,COL_TAG_KEY,new QTableWidgetItem());
      ui->tableWidget->item(i,COL_TAG_KEY)->setText(tag);
    }
  }

  ui->progressBar->setValue(0);
  ui->statusLabel->setText("Ready");
}

void BatchWindow::fileDropFinished(){
  qDebug("FDF");
  allowDrops = true;
  ui->runBatchButton->setEnabled(true);
  ui->tableWidget->resizeColumnsToContents();
  ui->tableWidget->resizeRowsToContents();
}

void BatchWindow::on_runBatchButton_clicked(){
  // Get a new preferences object in case they've changed since the last run.
  prefs = Preferences();

  checkRowsForSkipping();
  ui->runBatchButton->setEnabled(false);
  ui->cancelBatchButton->setEnabled(true);
  ui->tableWidget->setContextMenuPolicy(Qt::NoContextMenu); // so that no tags can be written while busy
  allowDrops = false;

  int numThreads = QThread::idealThreadCount();
  if(numThreads == -1 || prefs.getParallelBatchJobs() == false){
    QThreadPool::globalInstance()->setMaxThreadCount(1);
    ui->statusLabel->setText("Analysing (single thread)...");
  }else{
    QThreadPool::globalInstance()->setMaxThreadCount(numThreads);
    ui->statusLabel->setText("Analysing (" + QString::number(numThreads) + " threads)...");
  }

  runAnalysis();
}

void BatchWindow::checkRowsForSkipping(){
  bool skippingFiles = prefs.getSkipFilesWithExistingTags();
  bool commentRelevant = prefs.getWriteToTagComment();
  bool groupingRelevant = prefs.getWriteToTagGrouping();
  bool keyRelevant = prefs.getWriteToTagKey();
  int countRelevant = (commentRelevant ? 1 : 0) + (groupingRelevant ? 1 : 0) + (keyRelevant ? 1 : 0);
  for(int row = 0; row < ui->tableWidget->rowCount(); row++){

    // ignore files that are complete or failed
    QString status = ui->tableWidget->item(row,COL_STATUS)->text();
    if(status != STATUS_NEW && status != STATUS_TAGSREAD && status != STATUS_SKIPPED ){
      continue;
    }

    // if we're not skipping, or no tags are relevant, analyse everything
    if(!skippingFiles || (!commentRelevant && !groupingRelevant && !keyRelevant)){
      markRowSkipped(row,false);
      continue;
    }

    // otherwise, skip this file if all the relevant tags have data.
    // open question: what if tag is n/a?
    int countSkip = 0;
    if(commentRelevant && !(ui->tableWidget->item(row,COL_TAG_COMMENT) == 0))
      countSkip++;
    if(groupingRelevant && !(ui->tableWidget->item(row,COL_TAG_GROUPING) == 0))
      countSkip++;
    if(keyRelevant && !(ui->tableWidget->item(row,COL_TAG_KEY) == 0))
      countSkip++;

    if(countSkip == countRelevant){
      markRowSkipped(row,true);
    }else{
      markRowSkipped(row,false);
    }
  }
}

void BatchWindow::markRowSkipped(int row, bool skip){
  if(skip && ui->tableWidget->item(row,COL_STATUS)->text() != STATUS_SKIPPED){
    ui->tableWidget->item(row,COL_STATUS)->setText(STATUS_SKIPPED);
    ui->tableWidget->item(row,COL_KEY)->setText("Skipped");
    ui->tableWidget->item(row,COL_KEY)->setForeground(textError);
    return;
  }
  if(!skip && ui->tableWidget->item(row,COL_STATUS)->text() == STATUS_SKIPPED){
    ui->tableWidget->item(row,COL_STATUS)->setText(STATUS_TAGSREAD);
    ui->tableWidget->item(row,COL_KEY)->setText("");
    ui->tableWidget->item(row,COL_KEY)->setForeground(textDefault);
    return;
  }
}

void BatchWindow::runAnalysis(){
  QList<KeyFinderAnalysisObject> objects;
  for(int row = 0; row < ui->tableWidget->rowCount(); row++){
    QString status = ui->tableWidget->item(row,COL_STATUS)->text();
    if(status == STATUS_NEW || status == STATUS_TAGSREAD)
      objects.push_back(KeyFinderAnalysisObject(ui->tableWidget->item(row,COL_FILEPATH)->text(),prefs,row));
  }
  analysisFuture = QtConcurrent::mapped(objects, keyFinderProcessObject);
  analysisWatcher.setFuture(analysisFuture);
}

void BatchWindow::on_cancelBatchButton_clicked(){
  ui->statusLabel->setText("Cancelling...");
  ui->cancelBatchButton->setEnabled(false);
  analysisWatcher.cancel();

}

void BatchWindow::analysisCancelled(){
  cleanUpAfterRun();
}

void BatchWindow::cleanUpAfterRun(){
  allowDrops = true;
  ui->progressBar->setMaximum(1);
  ui->progressBar->setValue(0);
  ui->statusLabel->setText("Ready");
  ui->runBatchButton->setEnabled(true);
  ui->cancelBatchButton->setEnabled(false);
  ui->tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
  ui->tableWidget->resizeColumnsToContents();
}

void BatchWindow::progressRangeChanged(int minimum, int maximum){
  ui->progressBar->setMinimum(minimum);
  ui->progressBar->setMaximum(maximum);
}

void BatchWindow::progressValueChanged(int progressValue){
  ui->progressBar->setValue(progressValue);
}

void BatchWindow::resultReadyAt(int index){
  QString error = analysisWatcher.resultAt(index).errorMessage;
  int row = analysisWatcher.resultAt(index).batchRow;
  if(error == ""){
    int key = analysisWatcher.resultAt(index).globalKeyEstimate;
    ui->tableWidget->item(row,COL_STATUS)->setText(QString::number(key));
    ui->tableWidget->item(row,COL_KEY)->setText(prefs.getKeyCode(key));
    if(prefs.getWriteTagsAutomatically())
      writeToTagsAtRow(row);
  }else{
    ui->tableWidget->item(row,COL_STATUS)->setText(STATUS_FAILED);
    ui->tableWidget->item(row,COL_KEY)->setText("Failed: " + error);
    ui->tableWidget->item(row,COL_KEY)->setForeground(textError);
    ui->tableWidget->item(row,COL_FILENAME)->setForeground(textError);
  }
}

void BatchWindow::analysisFinished(){
  cleanUpAfterRun();
  QApplication::beep();
}

void BatchWindow::copySelectedFromTableWidget(){
  QByteArray copyArray;
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
      if(c != lastCol)
        copyArray.append("\t");
    }
    copyArray.append("\r\n");
  }
  QMimeData mimeData;
  mimeData.setData("text/plain",copyArray);
  QApplication::clipboard()->setMimeData(&mimeData);
}

void BatchWindow::writeDetectedToTags(){
  if(allowDrops){ // not an ideal check semantically, but it stops the user Cmd+T-ing during a batch run.
    // get a new preferences object in case they've changed since the last run.
    prefs = Preferences();
    // which files to write to
    int firstRow = INT_MAX;
    int lastRow = 0;
    foreach(QModelIndex selectedIndex,ui->tableWidget->selectionModel()->selectedIndexes()){
      int chkRow = selectedIndex.row();
      if(chkRow < firstRow) firstRow = chkRow;
      if(chkRow > lastRow) lastRow = chkRow;
    }
    // write
    int count = 0;
    for(int r = firstRow; r <= lastRow; r++){
      if(writeToTagsAtRow(r))
        count++;
    }
    QMessageBox msg;
    msg.setText("Data written to tags in " + QString::number(count) + " files");
    msg.exec();
  }
}

bool BatchWindow::writeToTagsAtRow(int row){
  // only write if there's a detected key
  bool toIntOk = false;
  int key = ui->tableWidget->item(row,COL_STATUS)->text().toInt(&toIntOk);
  if(!toIntOk || key < 0)
    return false;
  TagLibMetadata md(ui->tableWidget->item(row,COL_FILEPATH)->text());
  QString written = md.writeKeyToMetadata(key,prefs);
  if(written.isEmpty())
    return false;
  for(int i=0; i<(signed)written.size(); i++){
    int col = COL_TAG_COMMENT; // default
    if(written[i] == 'g') col = COL_TAG_GROUPING;
    if(written[i] == 'k') col = COL_TAG_KEY;
    if(ui->tableWidget->item(row,col) == 0)
      ui->tableWidget->setItem(row,col,new QTableWidgetItem());
    ui->tableWidget->item(row,col)->setText(prefs.getKeyCode(key));
    ui->tableWidget->item(row,col)->setForeground(textSuccess);
  }
  return true;
}

void BatchWindow::clearDetected(){
  int firstRow = INT_MAX;
  int lastRow = 0;
  foreach(QModelIndex selectedIndex,ui->tableWidget->selectionModel()->selectedIndexes()){
    int chkRow = selectedIndex.row();
    if(chkRow < firstRow) firstRow = chkRow;
    if(chkRow > lastRow) lastRow = chkRow;
  }
  for(int r = firstRow; r <= lastRow; r++){
    if(ui->tableWidget->item(r,COL_KEY) != 0){
      ui->tableWidget->item(r,COL_STATUS)->setText(STATUS_TAGSREAD);
      ui->tableWidget->item(r,COL_KEY)->setText("");
    }
  }
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
    QString msgText = "Please select a single row for detailed analysis";
    msg.setText(msgText);
    msg.exec();
    return;
  }
  menuHandler->new_Detail_Window(ui->tableWidget->item(firstRow,COL_FILEPATH)->text());
}
