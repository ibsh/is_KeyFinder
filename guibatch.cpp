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

const int COL_PLAYLIST_SOURCE = 0;
const int COL_PLAYLIST_NAME = 1;

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
  connect(&readLibraryWatcher, SIGNAL(finished()), this, SLOT(readLibraryFinished()));
  connect(&loadPlaylistWatcher, SIGNAL(finished()), this, SLOT(loadPlaylistFinished()));
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
  ui->libraryWidget->setColumnHidden(COL_PLAYLIST_SOURCE, true);
  ui->tableWidget->setColumnHidden(COL_FILEPATH, true);
  ui->tableWidget->setColumnHidden(COL_STATUS, true);
  ui->splitter->setStretchFactor(0, 1);
  ui->splitter->setStretchFactor(1, 3);
  ui->splitter->setCollapsible(0, true);
  ui->splitter->setCollapsible(1, false);
  if(prefs.getBatchWindowState() != QByteArray())
    this->restoreState(prefs.getBatchWindowState());
  if(prefs.getBatchWindowGeometry() != QByteArray())
    this->restoreGeometry(prefs.getBatchWindowGeometry());
  if(prefs.getBatchWindowSplitterState() != QByteArray())
    ui->splitter->restoreState(prefs.getBatchWindowSplitterState());
  keyFinderRow    = QBrush(QColor(191, 255, 191));
  keyFinderAltRow = QBrush(QColor(127, 234, 127));
  textDefault     = QBrush(QColor(  0,   0,   0));
  textSuccess     = QBrush(QColor(  0, 128,   0));
  textError       = QBrush(QColor(191,   0,   0));

  // Read music library
  ui->libraryWidget->insertRow(0);
  ui->libraryWidget->setItem(0, COL_PLAYLIST_SOURCE, new QTableWidgetItem());
  ui->libraryWidget->setItem(0,COL_PLAYLIST_NAME, new QTableWidgetItem());
  ui->libraryWidget->item(0, COL_PLAYLIST_SOURCE)->setText(SOURCE_KEYFINDER);
  ui->libraryWidget->item(0, COL_PLAYLIST_NAME)->setText("KeyFinder drag and drop");
  ui->libraryWidget->item(0,COL_PLAYLIST_NAME)->setIcon(QIcon(":/KeyFinder/images/icon-18.png"));
  ui->libraryWidget->item(0, COL_PLAYLIST_NAME)->setBackground(keyFinderRow);
  ui->libraryWidget->setColumnWidth(0,170);
  libraryOldIndex = 0;
  QFuture<QList<ExternalPlaylistObject> > readLibraryFuture = QtConcurrent::run(ExternalPlaylist::readLibrary, prefs);
  readLibraryWatcher.setFuture(readLibraryFuture);

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
  QAction* deleteSelectedRowsAction = new QAction(tr("Delete selected rows"),this);
  deleteSelectedRowsAction->setShortcut(QKeySequence::Delete);
  connect(deleteSelectedRowsAction, SIGNAL(triggered()), this, SLOT(deleteSelectedRows()));
  ui->tableWidget->addAction(deleteSelectedRowsAction);
  QAction* clearDetectedAction = new QAction(tr("Clear detected keys"),this);
  connect(clearDetectedAction, SIGNAL(triggered()), this, SLOT(clearDetected()));
  ui->tableWidget->addAction(clearDetectedAction);

  // VALGRIND
//  QList<QUrl> test;
//  test.push_back(QUrl("/Users/ibrahimshaath/Desktop/test tracks"));
//  receiveUrls(test);
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

void BatchWindow::closeEvent(QCloseEvent* e){
  // save ui settings, to an up-to-date prefs object
  prefs = Preferences();
  prefs.setBatchWindowState(this->saveState());
  prefs.setBatchWindowGeometry(this->saveGeometry());
  prefs.setBatchWindowSplitterState(ui->splitter->saveState());
  prefs.save();
  QMainWindow::closeEvent(e);
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

void BatchWindow::readLibraryFinished(){
  QList<ExternalPlaylistObject> libraryPlaylists = readLibraryWatcher.result();
  for(int i=0; i<(signed)libraryPlaylists.size(); i++){
    int newRow = ui->libraryWidget->rowCount();
    ui->libraryWidget->insertRow(newRow);
    ui->libraryWidget->setItem(newRow,COL_PLAYLIST_SOURCE, new QTableWidgetItem());
    ui->libraryWidget->setItem(newRow,COL_PLAYLIST_NAME, new QTableWidgetItem());
    ui->libraryWidget->item(newRow,COL_PLAYLIST_SOURCE)->setText(libraryPlaylists[i].source);
    ui->libraryWidget->item(newRow,COL_PLAYLIST_NAME)->setText(libraryPlaylists[i].name);
    QString iconPath = ":/KeyFinder/images/";
    if(libraryPlaylists[i].source == SOURCE_ITUNES){
      iconPath += "iTunes-18.png";
    }else if(libraryPlaylists[i].source == SOURCE_TRAKTOR){
      iconPath += "Traktor-18.png";
    }else if(libraryPlaylists[i].source == SOURCE_SERATO){
      iconPath += "Serato-18.png";
    }
    ui->libraryWidget->item(newRow,COL_PLAYLIST_NAME)->setIcon(QIcon(iconPath));
  }
  ui->libraryWidget->resizeRowsToContents();
}

void BatchWindow::on_libraryWidget_cellClicked(int row, int /*col*/){
  if(row == libraryOldIndex)
    return;
  if(ui->libraryWidget->item(libraryOldIndex, COL_PLAYLIST_SOURCE)->text() == SOURCE_KEYFINDER && ui->tableWidget->rowCount() > 0){
    QMessageBox msgBox;
    msgBox.setText("The drag and drop list will not be saved.");
    msgBox.setInformativeText("Are you sure you want to view another playlist?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    if(ret == QMessageBox::No){
      ui->libraryWidget->clearSelection();
      return;
    }
  }
  ui->tableWidget->setRowCount(0);
  this->setWindowTitle("KeyFinder - Batch Analysis");
  libraryOldIndex = row;
  if(ui->libraryWidget->item(row, COL_PLAYLIST_SOURCE)->text() == SOURCE_KEYFINDER)
    return;

  ui->libraryWidget->setEnabled(false);
  ui->statusLabel->setText("Loading playlist...");
  ui->runBatchButton->setEnabled(false);
  ui->cancelBatchButton->setEnabled(false);
  progressRangeChanged(0,0);
  progressValueChanged(-1);

  QString playlistName = ui->libraryWidget->item(row,COL_PLAYLIST_NAME)->text();
  QString playlistSource = ui->libraryWidget->item(row,COL_PLAYLIST_SOURCE)->text();
  QFuture<QList<QUrl> > loadPlaylistFuture = QtConcurrent::run(ExternalPlaylist::readLibraryPlaylist, playlistName, playlistSource, prefs);
  loadPlaylistWatcher.setFuture(loadPlaylistFuture);
}

void BatchWindow::loadPlaylistFinished(){
  if(loadPlaylistWatcher.result().size() > 0)
    receiveUrls(loadPlaylistWatcher.result());
  else
    setGuiDefaults();
}

void BatchWindow::dragEnterEvent(QDragEnterEvent *e){
  if(e->mimeData()->hasUrls()){
    e->acceptProposedAction();
  }
}

void BatchWindow::dropEvent(QDropEvent *e){
  if(ui->libraryWidget->currentIndex().row() != 0){
    QMessageBox msg;
    msg.setText("Cannot change an external playlist from KeyFinder");
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
    ui->runBatchButton->setEnabled(false);
    ui->libraryWidget->setEnabled(false);
    progressRangeChanged(0,0);
    progressValueChanged(-1);
    ui->statusLabel->setText("Loading files...");
    QFuture<void> addFileFuture = QtConcurrent::run(this,&BatchWindow::addDroppedFiles);
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
      droppedFiles << ExternalPlaylist::readM3uStandalonePlaylist(filePath);
      continue;
    }else if(fileExt == "xml"){
      droppedFiles << ExternalPlaylist::readITunesStandalonePlaylist(filePath);
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
  ui->cancelBatchButton->setEnabled(true);
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
  // VALGRIND
//  ui->runBatchButton->click();
}

void BatchWindow::on_runBatchButton_clicked(){
  // Get a new preferences object in case they've changed since the last run.
  prefs = Preferences();

  checkRowsForSkipping();
  ui->runBatchButton->setEnabled(false);
  ui->cancelBatchButton->setEnabled(true);
  ui->libraryWidget->setEnabled(false);
  ui->tableWidget->setContextMenuPolicy(Qt::NoContextMenu); // so that no tags can be written while busy
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
  QFuture<KeyFinderResultWrapper> analysisFuture = QtConcurrent::mapped(objects, keyDetectionProcess);
  analysisWatcher.setFuture(analysisFuture);
}

void BatchWindow::on_cancelBatchButton_clicked(){
  ui->statusLabel->setText("Cancelling...");
  ui->cancelBatchButton->setEnabled(false);
  progressRangeChanged(0,0);
  progressValueChanged(-1);
  metadataReadWatcher.cancel();
  analysisWatcher.cancel();
}

void BatchWindow::analysisResultReadyAt(int index){
  QString error = analysisWatcher.resultAt(index).errorMessage;
  int row = analysisWatcher.resultAt(index).batchRow;
  if(error == ""){
    int key = analysisWatcher.resultAt(index).core.globalKeyEstimate;
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
  // VALGRIND
//  this->close();
}

void BatchWindow::writeDetectedToTags(){
  if(addFilesWatcher.isRunning() || metadataReadWatcher.isRunning() || analysisWatcher.isRunning()){
    QApplication::beep();
    return;
  }
  // get a new preferences object in case they've changed since the last run.
  prefs = Preferences();
  // which files to write to
  int successfullyTagged = 0;
  std::vector<int> rowsTried;
  foreach(QModelIndex selectedIndex,ui->tableWidget->selectionModel()->selectedIndexes()){
    int chkRow = selectedIndex.row();
    if(std::find(rowsTried.begin(), rowsTried.end(), chkRow) == rowsTried.end()){
      if(writeToTagsAtRow(chkRow))
        successfullyTagged++;
      rowsTried.push_back(chkRow);
    }
  }
  QMessageBox msg;
  msg.setText("Data written to tags in " + QString::number(successfullyTagged) + " files");
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
  std::vector<int> rowsCleared;
  foreach(QModelIndex selectedIndex,ui->tableWidget->selectionModel()->selectedIndexes()){
    int chkRow = selectedIndex.row();
    if(std::find(rowsCleared.begin(), rowsCleared.end(), chkRow) == rowsCleared.end()){
      if(ui->tableWidget->item(chkRow,COL_KEY) != 0){
        ui->tableWidget->item(chkRow,COL_STATUS)->setText(STATUS_TAGSREAD);
        ui->tableWidget->item(chkRow,COL_KEY)->setText("");
      }
      rowsCleared.push_back(chkRow);
    }
  }
}

void BatchWindow::deleteSelectedRows(){
  if(ui->libraryWidget->currentIndex().row() != 0){
    QMessageBox msg;
    msg.setText("Cannot change an external playlist from KeyFinder");
    msg.exec();
    return;
  }
  std::vector<int> rowsToDelete;
  foreach(QModelIndex selectedIndex,ui->tableWidget->selectionModel()->selectedIndexes()){
    int chkRow = selectedIndex.row();
    if(std::find(rowsToDelete.begin(), rowsToDelete.end(), chkRow) == rowsToDelete.end()){
      rowsToDelete.push_back(chkRow);
    }
  }
  for(int r = ui->tableWidget->rowCount(); r >= 0 ; r--){
    if(std::find(rowsToDelete.begin(), rowsToDelete.end(), r) != rowsToDelete.end()){
      ui->tableWidget->removeRow(r);
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
