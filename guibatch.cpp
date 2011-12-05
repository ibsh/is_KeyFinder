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

const int COL_PATH = 0;
const int COL_TAG_ARTIST = 1;
const int COL_TAG_TITLE = 2;
const int COL_TAG_COMMENT = 3;
const int COL_TAG_GROUPING = 4;
const int COL_TAG_KEY = 5;
const int COL_KEY = 6;
const int COL_KEYCODE = 7;

BatchWindow::BatchWindow(MainMenuHandler* handler, QWidget* parent) : QMainWindow(parent), ui(new Ui::BatchWindow){
  // ASYNC
  connect(&fileDropWatcher, SIGNAL(finished()), this, SLOT(fileDropFinished()));
  modelThread = NULL;
  // SETUP UI
  ui->setupUi(this);
  allowDrops = true;
  vis = Visuals::getInstance();
  menuHandler = handler;
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
  fileDropWatcher.cancel();
  fileDropWatcher.waitForFinished();
  if(modelThread!=NULL && modelThread->isRunning()){
    modelThread->quit();
    modelThread->wait();
  }
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
  ui->statusLabel->setText("Loading files...");
  QFuture<void> future = QtConcurrent::run(this,&BatchWindow::filesDropped,e->mimeData()->urls());
  fileDropWatcher.setFuture(future);
}

QStringList BatchWindow::getDirectoryContents(QDir dir){
  QFileInfoList contents = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst);
  QStringList results;
  for(int i = 0; i<(signed)contents.size(); i++){
    if(QFileInfo(contents[i].filePath()).isDir()){
      results += getDirectoryContents(QDir(contents[i].filePath()));
    }else{
      results.push_back(QUrl::fromLocalFile(contents[i].filePath()).toString());
    }
  }
  return results;
}

void BatchWindow::filesDropped(QList<QUrl>& urls){
  for(int i=0; i<urls.size(); i++){
    QString fileUrl = urls[i].toLocalFile();
    // check if url is a directory; if so, get contents rather than adding
    if(QFileInfo(fileUrl).isDir()){
      QStringList contents = getDirectoryContents(QDir(fileUrl));
      for(int j=0; j<contents.size(); j++)
        urls.push_back(QUrl(contents[j]));
      continue;
    }
    // check if path is already in the list
    bool isNew = true;
    for(int j=0; j<ui->tableWidget->rowCount(); j++){
      if(ui->tableWidget->item(j,COL_PATH)->text() == fileUrl){
        isNew = false;
        break;
      }
    }
    if(isNew){
      addNewRow(fileUrl);
    }
  }
  this->setWindowTitle("KeyFinder - Batch Analysis - " + QString::number(ui->tableWidget->rowCount()) + " files");
  getMetadata();
}

void BatchWindow::addNewRow(QString fileUrl){
  QString fileExt = fileUrl.right(3);
  if(fileExt == "m3u"){
    loadPlaylistM3u(fileUrl);
    return;
  }else if(fileExt == "xml"){
    loadPlaylistXml(fileUrl);
    return;
  }
  if(initialHelpLabel != NULL){
    delete initialHelpLabel;
    initialHelpLabel = NULL;
  }
  int newRow = ui->tableWidget->rowCount();
  ui->tableWidget->insertRow(newRow);
  ui->tableWidget->setItem(newRow,COL_PATH,new QTableWidgetItem());
  ui->tableWidget->item(newRow,COL_PATH)->setText(fileUrl);
}

void BatchWindow::loadPlaylistM3u(QString m3uUrl){
  QFile m3uFile(m3uUrl);
  if(!m3uFile.open(QIODevice::ReadOnly))
    return;
  QTextStream m3uTextStream(&m3uFile);
  QString m3uChar;
  QString m3uLine;
  QList<QUrl> songUrls;
  // M3U files break with ch10/13, and comment with ch35.
  // QTextStream.readLine doesn't work, so we do it a char at a time
  while(!(m3uChar = m3uTextStream.read(1)).isNull()){
    //std::cerr << m3uChar.toAscii().data() << ":" << int(m3uChar[0].toAscii()) << std::endl;
    int chVal = int(m3uChar[0].toAscii());
    if(chVal == 13 || chVal == 10){
      //std::cerr << "Line (length " << m3uLine.length() << "): " << m3uLine.toUtf8().data() << std::endl;
      if(m3uLine.length() > 0 && int(m3uLine[0].toAscii()) != 35){
        songUrls.push_back(QUrl(m3uLine));
      }
      m3uLine = "";
    }else{
      m3uLine += m3uChar;
    }
  }
  filesDropped(songUrls);
}

void BatchWindow::loadPlaylistXml(QString /*xmlUrl*/){
  qDebug("XML playlists not working yet");
  /*
  QFile xmlFile(xmlUrl);
  if (!xmlFile.open(QIODevice::ReadOnly))
    return;

  QXmlQuery xq;
  xq.bindVariable("inputDocument", &xmlFile);
  xq.setQuery("doc($inputDocument)/plist/dict/dict/dict");

  xmlFile.close();
  */
  /*
  QDomDocument domDoc("whatevs");
  // path is plist -> dict -> dict -> each dict -> string element after key element = location
  QDomNode n = domDoc.documentElement().firstChild().firstChild(); // plist -> dict
  while(!n.isNull()) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if(!e.isNull() && e.tagName() == "dict") {
      n = n.firstChild(); // plist -> dict -> dict
      break;
    }
    n = n.nextSibling();
  }
  */
}

void BatchWindow::getMetadata(){
  ui->progressBar->setMaximum(ui->tableWidget->rowCount());
  ui->statusLabel->setText("Reading tags...");
  for(int i=0; i<(signed)ui->tableWidget->rowCount(); i++){
    ui->progressBar->setValue(i);
    if(ui->tableWidget->item(i,COL_TAG_ARTIST) != NULL)
      continue;
    TagLibMetadata* md = new TagLibMetadata(ui->tableWidget->item(i,COL_PATH)->text());
    QString tag = md->getArtist();
    // create an item for artist whether tagging worked or not
    ui->tableWidget->setItem(i,COL_TAG_ARTIST,new QTableWidgetItem());
    ui->tableWidget->item(i,COL_TAG_ARTIST)->setText(tag);
    tag = md->getTitle();
    if(tag != ""){
      ui->tableWidget->setItem(i,COL_TAG_TITLE,new QTableWidgetItem());
      ui->tableWidget->item(i,COL_TAG_TITLE)->setText(tag);
    }
    tag = md->getComment();
    if(tag != ""){
      ui->tableWidget->setItem(i,COL_TAG_COMMENT,new QTableWidgetItem());
      ui->tableWidget->item(i,COL_TAG_COMMENT)->setText(tag);
    }
    tag = md->getGrouping();
    if(tag != ""){
      ui->tableWidget->setItem(i,COL_TAG_GROUPING,new QTableWidgetItem());
      ui->tableWidget->item(i,COL_TAG_GROUPING)->setText(tag);
    }
    tag = md->getKey();
    if(tag != ""){
      ui->tableWidget->setItem(i,COL_TAG_KEY,new QTableWidgetItem());
      ui->tableWidget->item(i,COL_TAG_KEY)->setText(tag);
    }
    delete md;
  }
  ui->progressBar->setValue(0);
  ui->statusLabel->setText("Ready");
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
  ui->cancelBatchButton->setDisabled(false);
  cancel = false;
  ui->tableWidget->setContextMenuPolicy(Qt::NoContextMenu); // so that no tags can be written while busy
  allowDrops = false;
  ui->statusLabel->setText("Analysing...");
  ui->progressBar->setMaximum(ui->tableWidget->rowCount());
  currentFile = 0;
  processCurrentFile();
}

void BatchWindow::on_cancelBatchButton_clicked(){
  cancel = true;
  ui->statusLabel->setText("Cancelling...");
  ui->cancelBatchButton->setDisabled(true);
}

void BatchWindow::processCurrentFile(){
  if(cancel || currentFile == ui->tableWidget->rowCount()){
    cleanUpAfterRun();
    QApplication::beep();
    return;
  }else if(ui->tableWidget->item(currentFile,COL_KEY) != NULL){
    // don't reprocess files
    currentFile++;
    processCurrentFile();
  }else{
    qDebug("Batch processing %s",ui->tableWidget->item(currentFile,COL_PATH)->text().toUtf8().data());
    ui->progressBar->setValue(currentFile);
    // now proceed
    modelThread = new KeyFinderWorkerThread(0);
    modelThread->setParams(ui->tableWidget->item(currentFile,COL_PATH)->text(),prefs);
    connect(modelThread,SIGNAL(failed(QString)),this,SLOT(fileFailed()));
    connect(modelThread,SIGNAL(producedGlobalKeyEstimate(int)),this,SLOT(fileFinished(int)));
    modelThread->start();
  }
}

void BatchWindow::cleanUpAfterRun(){
  delete modelThread;
  modelThread = NULL;
  allowDrops = true;
  ui->progressBar->setValue(0);
  ui->statusLabel->setText("Ready");
  ui->runBatchButton->setDisabled(false);
  ui->cancelBatchButton->setDisabled(true);
  ui->tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
  ui->tableWidget->resizeColumnsToContents();
}

void BatchWindow::fileFinished(int key){
  ui->tableWidget->setItem(currentFile,COL_KEY,new QTableWidgetItem());
  ui->tableWidget->item(currentFile,COL_KEY)->setText(vis->getKeyName(key));
  ui->tableWidget->setItem(currentFile,COL_KEYCODE,new QTableWidgetItem());
  ui->tableWidget->item(currentFile,COL_KEYCODE)->setText(prefs.getCustomKeyCodes()[key]);
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
      QTableWidgetItem* item = ui->tableWidget->item(r,COL_KEY); // only write if there's a detected key
      if(item != NULL && item->text() != "Failed"){
        TagLibMetadata md(ui->tableWidget->item(r,COL_PATH)->text().toUtf8().data());
        QString writeToTag = "";
        // what are we writing?
        if(prefs.getTagFormat() == 'k'){
          writeToTag = ui->tableWidget->item(r,COL_KEY)->text();
        }else if(prefs.getTagFormat() == 'c'){
          writeToTag = ui->tableWidget->item(r,COL_KEYCODE)->text();
        }else{
          writeToTag = ui->tableWidget->item(r,COL_KEYCODE)->text() + " " + ui->tableWidget->item(r,COL_KEY)->text();
        }
        // where are we writing it?
        if(prefs.getTagField() == 'g'){
          if(md.setGrouping(writeToTag.toUtf8().data()) == 0)
            count++;
        }else if(prefs.getTagField() == 'k'){
          if(md.setKey(writeToTag.left(3).toUtf8().data()) == 0)
            count++;
        }else{
          if(md.setComment(writeToTag.toUtf8().data()) == 0)
            count++;
        }
      }
    }
    QMessageBox msg;
    QString msgText = "Data written to ";
    if(prefs.getTagField() == 'g')
      msgText += "grouping";
    else if(prefs.getTagField() == 'k')
      msgText += "key";
    else
      msgText += "comment";
    msgText += " tag in ";
    msgText += QString("%1").arg(count);
    msgText += " files";
    msg.setText(msgText);
    msg.exec();
  }
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
    if(ui->tableWidget->item(r,COL_KEY) != NULL){
      delete ui->tableWidget->item(r,COL_KEY);
      delete ui->tableWidget->item(r,COL_KEYCODE);
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
  menuHandler->new_Detail_Window(ui->tableWidget->item(firstRow,COL_PATH)->text());
}
