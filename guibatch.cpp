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
  connect(&addFilesWatcher, SIGNAL(finished()), this, SLOT(addFilesFinished()));

  connect(&metadataReadWatcher, SIGNAL(resultReadyAt(int)), this, SLOT(metadataReadResultReadyAt(int)));
  connect(&metadataReadWatcher, SIGNAL(finished()), this, SLOT(metadataReadFinished()));
  connect(&metadataReadWatcher, SIGNAL(progressRangeChanged(int, int)), this, SLOT(progressRangeChanged(int,int)));
  connect(&metadataReadWatcher, SIGNAL(progressValueChanged(int)), this, SLOT(progressValueChanged(int)));

  connect(&analysisWatcher, SIGNAL(resultReadyAt(int)), this, SLOT(analysisResultReadyAt(int)));
  connect(&analysisWatcher, SIGNAL(finished()), this, SLOT(analysisFinished())); // takes care of cancelled too
  connect(&analysisWatcher, SIGNAL(progressRangeChanged(int, int)), this, SLOT(progressRangeChanged(int,int)));
  connect(&analysisWatcher, SIGNAL(progressValueChanged(int)), this, SLOT(progressValueChanged(int)));

  // SETUP UI
  ui->setupUi(this);
  menuHandler = handler;
  ui->tableWidget->setColumnHidden(COL_STATUS,true);
  ui->tableWidget->setColumnHidden(COL_FILEPATH,true);
  keyFinderRow = QBrush(QColor(191,255,191));
  keyFinderAltRow = QBrush(QColor(127,234,127));
  iTunesRow = QBrush(QColor(223,223,255));
  traktorRow = QBrush(QColor(191,191,191));
  seratoRow = QBrush(QColor(255,191,191));
  textDefault = QBrush(QColor(0,0,0));
  textSuccess = QBrush(QColor(0,128,0));
  textError = QBrush(QColor(191,0,0));

  ui->libraryWidget->insertRow(0);
  ui->libraryWidget->setItem(0,0,new QTableWidgetItem());
  ui->libraryWidget->item(0,0)->setText("KeyFinder drag and drop");
  ui->libraryWidget->item(0,0)->setBackground(keyFinderRow);
  ui->libraryWidget->setColumnWidth(0,170);
  libraryOldIndex = 0;
  loadITunesPlaylistsIntoListWidget();

  //relative sizing on Mac only
#ifdef Q_OS_MAC
  QFont smallerFont;
  smallerFont.setPointSize(smallerFont.pointSize() - 2);
  ui->tableWidget->setFont(smallerFont);
  ui->libraryWidget->setFont(smallerFont);
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
        (494 - initialHelpLabel->sizeHint().width()) / 2,
        (334 - initialHelpLabel->sizeHint().height()) / 2,
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
  addFilesWatcher.cancel();
  metadataReadWatcher.cancel();
  analysisWatcher.cancel();
  addFilesWatcher.waitForFinished();
  metadataReadWatcher.waitForFinished();
  analysisWatcher.waitForFinished();
  delete ui;
}

void BatchWindow::setThreadCount(){
  int numThreads = QThread::idealThreadCount();
  if(numThreads == -1 || prefs.getParallelBatchJobs() == false){
    QThreadPool::globalInstance()->setMaxThreadCount(1);
  }else{
    QThreadPool::globalInstance()->setMaxThreadCount(numThreads);
  }
}

void BatchWindow::setGuiDefaults(){
  progressRangeChanged(0,100);
  progressValueChanged(0);
  ui->statusLabel->setText("Ready");
  ui->runBatchButton->setEnabled(true);
  ui->cancelBatchButton->setEnabled(false);
  ui->libraryWidget->setEnabled(true);
  ui->tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
  ui->tableWidget->resizeColumnsToContents();
  ui->tableWidget->resizeRowsToContents();
}

void BatchWindow::loadITunesPlaylistsIntoListWidget(){
  QStringList playLists;
  QFile xmlFile(prefs.getITunesLibraryPath());
  if (!xmlFile.open(QIODevice::ReadOnly))
    return;

  QXmlQuery xmlQuery;
  xmlQuery.bindVariable("inputDocument", &xmlFile);

  QString xPath;
  xPath += "doc($inputDocument)/plist/dict";
  xPath += "/array[preceding-sibling::key[1]='Playlists']";
  xPath += "/dict/string[preceding-sibling::key[1]='Name']/string(text())";

  xmlQuery.setQuery(xPath);

  xmlQuery.evaluateTo(&playLists);
  xmlFile.close();

  if(playLists.isEmpty())
    return;

  for(int i=0; i<(signed)playLists.size(); i++){
    int newRow = ui->libraryWidget->rowCount();
    ui->libraryWidget->insertRow(newRow);
    ui->libraryWidget->setItem(newRow,0,new QTableWidgetItem());
    ui->libraryWidget->item(newRow,0)->setText(playLists[i]);
    ui->libraryWidget->item(newRow,0)->setBackground(iTunesRow);
  }
  ui->libraryWidget->resizeRowsToContents();
}

void BatchWindow::on_libraryWidget_cellClicked(int row, int col){
  if(row == libraryOldIndex)
    return;
  if(libraryOldIndex == 0 && ui->tableWidget->rowCount() > 0){
    QMessageBox msgBox;
    msgBox.setText("The drag and drop list will not be saved.");
    msgBox.setInformativeText("Are you sure you want to view another playlist?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    if(ret == QMessageBox::No){
      ui->libraryWidget->selectRow(0);
      return;
    }
  }
  ui->tableWidget->setRowCount(0);
  this->setWindowTitle("KeyFinder - Batch Analysis");
  libraryOldIndex = row;
  if(libraryOldIndex != 0)
    loadITunesPlaylistIntoTableWidget(ui->libraryWidget->item(row,col)->text());
}

void BatchWindow::loadITunesPlaylistIntoTableWidget(QString playList){

  QStringList trackIds;
  QList<QUrl> results;

  QFile xmlFile(prefs.getITunesLibraryPath());
  if (!xmlFile.open(QIODevice::ReadOnly))
    return;
  QXmlQuery xmlQuery;
  xmlQuery.bindVariable("inputDocument", &xmlFile);

  QString xPath;
  xPath += "doc($inputDocument)/plist/dict";
  xPath += "/array[preceding-sibling::key[1]='Playlists']";
  xPath += "/dict[child::string[preceding-sibling::key[1]='Name'][1]='";
  xPath +=  playList;
  xPath += "']";
  xPath += "/array/dict/integer[preceding-sibling::key[1]='Track ID']";
  xPath += "/string(text())";

  xmlQuery.setQuery(xPath);
  xmlQuery.evaluateTo(&trackIds);

  for(int i = 0; i < (signed)trackIds.size(); i++){

    QStringList resultStrings; // if you use a single string it breaks, for some reason

    xPath = "";
    xPath += "doc($inputDocument)/plist/dict";
    xPath += "/dict[preceding-sibling::key[1]='Tracks']";
    xPath += "/dict[preceding-sibling::key[1]='";
    xPath += trackIds[i];
    xPath += "']";
    xPath += "/string[preceding-sibling::key[1]='Location']/string(text())";

    xmlQuery.setQuery(xPath);
    if(xmlQuery.evaluateTo(&resultStrings)){
      // replace iTunes' localhost addressing.
      results.push_back(QUrl(resultStrings[0].replace(QString("//localhost"),QString("")).toLocal8Bit()));
    }
  }

  xmlFile.close();
  receiveUrls(results);
}

void BatchWindow::dragEnterEvent(QDragEnterEvent *e){
  if(e->mimeData()->hasUrls()){
    e->acceptProposedAction();
  }
}

void BatchWindow::dropEvent(QDropEvent *e){
  if(ui->libraryWidget->currentIndex().row() != 0){
    QMessageBox msg;
    msg.setText("Cannot change an iTunes playlist from KeyFinder");
    msg.exec();
    return;
  }
  receiveUrls(e->mimeData()->urls());
}

bool BatchWindow::receiveUrls(const QList<QUrl>& urls){
  if(metadataReadWatcher.isRunning() || analysisWatcher.isRunning())
    return false;
  droppedFiles << urls;
  if(!addFilesWatcher.isRunning()){
    // Get a new preferences object in case they've changed since the last run.
    prefs = Preferences();
    setThreadCount();
    ui->runBatchButton->setEnabled(false);
    ui->libraryWidget->setEnabled(false);
    ui->statusLabel->setText("Loading files...");
    QFuture<void>addFileFuture = QtConcurrent::run(this,&BatchWindow::addDroppedFiles);
    addFilesWatcher.setFuture(addFileFuture);
  }
  return true;
}

void BatchWindow::addDroppedFiles(){
  while(!droppedFiles.isEmpty()){

    // check URL resolves to local file
    QString filePath = droppedFiles.first().toLocalFile();
    droppedFiles.removeFirst();
    if(filePath.isEmpty())
      continue;

    // check for directory
    QFileInfo fileInfo(filePath);
    if(fileInfo.isDir()){
      droppedFiles << getDirectoryContents(QDir(filePath));
      continue;
    }

    // check for symlink (.isSymLink doesn't seem to work on Lion)
    if(fileInfo.isSymLink() || fileInfo.symLinkTarget() != ""){
      droppedFiles.push_back(QUrl(fileInfo.symLinkTarget()));
      continue;
    }

    // check for playlist
    QString fileExt = filePath.right(3);
    if(fileExt == "m3u"){
      droppedFiles << loadPlaylistM3u(filePath);
      continue;
    }else if(fileExt == "xml"){
      droppedFiles << loadPlaylistXml(filePath);
      continue;
    }

    // check for duplicate
    bool isNew = true;
    for(int j=0; j<(signed)ui->tableWidget->rowCount(); j++){
      if(ui->tableWidget->item(j,COL_FILEPATH)->text() == filePath){
        isNew = false;
        break;
      }
    }

    if(isNew)
      addNewRow(filePath);

  }
  this->setWindowTitle("KeyFinder - Batch Analysis - " + QString::number(ui->tableWidget->rowCount()) + " files");
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
  QStringList resultStrings;
  QList<QUrl> results;
  QFile xmlFile(xmlFileUrl);
  if (!xmlFile.open(QIODevice::ReadOnly))
    return results;

  QXmlQuery xmlQuery;
  xmlQuery.bindVariable("inputDocument", &xmlFile);
  xmlQuery.setQuery("doc($inputDocument)/plist/dict/dict[preceding-sibling::key[1]='Tracks']/dict/string[preceding-sibling::key[1]='Location']/string(text())");

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
  ui->tableWidget->item(newRow,COL_FILENAME)->setText(fileUrl.mid(fileUrl.lastIndexOf("/") + 1)); // note forwardslash not QDir::separator
  ui->tableWidget->setItem(newRow,COL_KEY,new QTableWidgetItem());
  if(newRow % 2 == 0){
    ui->tableWidget->item(newRow,COL_KEY)->setBackground(keyFinderRow);
  }else{
    ui->tableWidget->item(newRow,COL_KEY)->setBackground(keyFinderAltRow);
  }
}

void BatchWindow::addFilesFinished(){
  readMetadata();
}

void BatchWindow::readMetadata(){
  ui->statusLabel->setText("Reading tags...");
  QList<AsyncFileObject> objects;
  for(int row = 0; row < (signed)ui->tableWidget->rowCount(); row++){
    if(ui->tableWidget->item(row,COL_STATUS)->text() == STATUS_NEW)
      objects.push_back(AsyncFileObject(ui->tableWidget->item(row,COL_FILEPATH)->text(),prefs,row));
  }
  QFuture<MetadataReadResult> metadataReadFuture = QtConcurrent::mapped(objects, metadataReadProcess);
  metadataReadWatcher.setFuture(metadataReadFuture);
}

void BatchWindow::metadataReadResultReadyAt(int index){
  int row = metadataReadWatcher.resultAt(index).batchRow;
  if(metadataReadWatcher.resultAt(index).artist != ""){
    ui->tableWidget->setItem(row,COL_TAG_ARTIST,new QTableWidgetItem());
    ui->tableWidget->item(row,COL_TAG_ARTIST)->setText(metadataReadWatcher.resultAt(index).artist);
  }
  if(metadataReadWatcher.resultAt(index).title != ""){
    ui->tableWidget->setItem(row,COL_TAG_TITLE,new QTableWidgetItem());
    ui->tableWidget->item(row,COL_TAG_TITLE)->setText(metadataReadWatcher.resultAt(index).title);
  }
  if(metadataReadWatcher.resultAt(index).comment != ""){
    ui->tableWidget->setItem(row,COL_TAG_COMMENT,new QTableWidgetItem());
    ui->tableWidget->item(row,COL_TAG_COMMENT)->setText(metadataReadWatcher.resultAt(index).comment);
  }
  if(metadataReadWatcher.resultAt(index).grouping != ""){
    ui->tableWidget->setItem(row,COL_TAG_GROUPING,new QTableWidgetItem());
    ui->tableWidget->item(row,COL_TAG_GROUPING)->setText(metadataReadWatcher.resultAt(index).grouping);
  }
  if(metadataReadWatcher.resultAt(index).key != ""){
    ui->tableWidget->setItem(row,COL_TAG_KEY,new QTableWidgetItem());
    ui->tableWidget->item(row,COL_TAG_KEY)->setText(metadataReadWatcher.resultAt(index).key);
  }
}

void BatchWindow::metadataReadFinished(){
  setGuiDefaults();
}

void BatchWindow::on_runBatchButton_clicked(){
  // Get a new preferences object in case they've changed since the last run.
  prefs = Preferences();

  checkRowsForSkipping();
  ui->runBatchButton->setEnabled(false);
  ui->cancelBatchButton->setEnabled(true);
  ui->libraryWidget->setEnabled(false);
  ui->tableWidget->setContextMenuPolicy(Qt::NoContextMenu); // so that no tags can be written while busy

  setThreadCount();
  ui->statusLabel->setText("Analysing (" + QString::number(QThreadPool::globalInstance()->maxThreadCount()) + " threads)...");

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
  QList<AsyncFileObject> objects;
  for(int row = 0; row < ui->tableWidget->rowCount(); row++){
    QString status = ui->tableWidget->item(row,COL_STATUS)->text();
    if(status == STATUS_NEW || status == STATUS_TAGSREAD)
      objects.push_back(AsyncFileObject(ui->tableWidget->item(row,COL_FILEPATH)->text(),prefs,row));
  }
  QFuture<KeyDetectionResult> analysisFuture = QtConcurrent::mapped(objects, keyDetectionProcess);
  analysisWatcher.setFuture(analysisFuture);
}

void BatchWindow::on_cancelBatchButton_clicked(){
  ui->statusLabel->setText("Cancelling...");
  ui->cancelBatchButton->setEnabled(false);
  progressRangeChanged(0,0);
  progressValueChanged(-1);
  analysisWatcher.cancel();
}

void BatchWindow::analysisResultReadyAt(int index){
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
  setGuiDefaults();
  QApplication::beep();
}

void BatchWindow::writeDetectedToTags(){
  if(addFilesWatcher.isRunning() || metadataReadWatcher.isRunning() || analysisWatcher.isRunning()){
    QApplication::beep();
    return;
  }
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
    msg.setText("Please select a single row for detailed analysis");
    msg.exec();
    return;
  }
  menuHandler->new_Detail_Window(ui->tableWidget->item(firstRow,COL_FILEPATH)->text());
}

void BatchWindow::progressRangeChanged(int minimum, int maximum){
  ui->progressBar->setMinimum(minimum);
  ui->progressBar->setMaximum(maximum);
}

void BatchWindow::progressValueChanged(int progressValue){
  ui->progressBar->setValue(progressValue);
}

void BatchWindow::checkForNewVersion(){
  QNetworkAccessManager *manager = new QNetworkAccessManager(this);
  connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(receiveNetworkReply(QNetworkReply*)));
  manager->get(QNetworkRequest(QUrl("http://www.ibrahimshaath.co.uk/keyfinder/kf.xml")));
}

void BatchWindow::receiveNetworkReply(QNetworkReply* reply){
  QString newVersion = "";
  if(reply->error() == QNetworkReply::NoError){
    QXmlQuery xmlQuery;
    xmlQuery.setFocus(reply->readAll());
    xmlQuery.setQuery("//KeyFinder/version/@*/string(.)");
    QStringList xmlContents;
    xmlQuery.evaluateTo(&xmlContents);
    if(!xmlContents.isEmpty()){
      int latest_major = xmlContents[0].toInt();
      int latest_minor = xmlContents[1].toInt();
      if(latest_major > VERSION_MAJOR || latest_minor > VERSION_MINOR)
        newVersion = xmlContents[0] + "." + xmlContents[1];
    }
  }
  reply->deleteLater();
  if(!newVersion.isEmpty()){
    newVersion = "A new version, v" + newVersion + ", is available on <a href='http://www.ibrahimshaath.co.uk/keyfinder/'>the KeyFinder website</a>!";
    QMessageBox msg;
    msg.setText(newVersion);
    msg.exec();
  }
}
