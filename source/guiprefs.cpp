/*************************************************************************

  Copyright 2011-2015 Ibrahim Sha'ath

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

#include "guiprefs.h"
#include "ui_prefsdialog.h"

PrefsDialog::PrefsDialog(QWidget *parent): QDialog(parent),ui(new Ui::PrefsDialog) {

  // map dropdown values to indices
  listMetadataWrite << METADATA_WRITE_NONE;
  listMetadataWrite << METADATA_WRITE_PREPEND;
  listMetadataWrite << METADATA_WRITE_APPEND;
  listMetadataWrite << METADATA_WRITE_OVERWRITE; // out of range for some fields
  listMetadataWriteKey << METADATA_WRITE_NONE;
  listMetadataWriteKey << METADATA_WRITE_OVERWRITE;
  listMetadataFormat << METADATA_FORMAT_KEYS;
  listMetadataFormat << METADATA_FORMAT_CUSTOM;
  listMetadataFormat << METADATA_FORMAT_BOTH;

  // UI
  ui->setupUi(this);
  this->setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);

  // get values from preferences
  Preferences p;
  ui->writeToFilesAutomatically->setChecked(p.getWriteToFilesAutomatically());
  ui->parallelBatchJobs->setChecked(p.getParallelBatchJobs());
  ui->skipFilesWithExistingTags->setChecked(p.getSkipFilesWithExistingTags());
  ui->applyFileExtensionFilter->setChecked(p.getApplyFileExtensionFilter());
  ui->maxDuration->setValue(p.getMaxDuration());

  ui->tagFormat->setCurrentIndex(listMetadataFormat.indexOf(p.getMetadataFormat()));
  ui->metadataWriteTitle->setCurrentIndex(listMetadataWrite.indexOf(p.getMetadataWriteTitle()));
  ui->metadataWriteArtist->setCurrentIndex(listMetadataWrite.indexOf(p.getMetadataWriteArtist()));
  ui->metadataWriteAlbum->setCurrentIndex(listMetadataWrite.indexOf(p.getMetadataWriteAlbum()));
  ui->metadataWriteComment->setCurrentIndex(listMetadataWrite.indexOf(p.getMetadataWriteComment()));
  ui->metadataWriteGrouping->setCurrentIndex(listMetadataWrite.indexOf(p.getMetadataWriteGrouping()));
  ui->metadataWriteKey->setCurrentIndex(listMetadataWriteKey.indexOf(p.getMetadataWriteKey()));
  ui->metadataWriteFilename->setCurrentIndex(listMetadataWrite.indexOf(p.getMetadataWriteFilename()));
  ui->metadataDelimiter->setText(p.getMetadataDelimiter());

  ui->iTunesLibraryPath->setText(p.getITunesLibraryPath());
  ui->traktorLibraryPath->setText(p.getTraktorLibraryPath());
  ui->seratoLibraryPath->setText(p.getSeratoLibraryPath());

  QStringList ckc = p.getCustomKeyCodes();
  ui->majKey0->setText(ckc[0]);   ui->minKey0->setText(ckc[1]);
  ui->majKey1->setText(ckc[2]);   ui->minKey1->setText(ckc[3]);
  ui->majKey2->setText(ckc[4]);   ui->minKey2->setText(ckc[5]);
  ui->majKey3->setText(ckc[6]);   ui->minKey3->setText(ckc[7]);
  ui->majKey4->setText(ckc[8]);   ui->minKey4->setText(ckc[9]);
  ui->majKey5->setText(ckc[10]);  ui->minKey5->setText(ckc[11]);
  ui->majKey6->setText(ckc[12]);  ui->minKey6->setText(ckc[13]);
  ui->majKey7->setText(ckc[14]);  ui->minKey7->setText(ckc[15]);
  ui->majKey8->setText(ckc[16]);  ui->minKey8->setText(ckc[17]);
  ui->majKey9->setText(ckc[18]);  ui->minKey9->setText(ckc[19]);
  ui->majKey10->setText(ckc[20]); ui->minKey10->setText(ckc[21]);
  ui->majKey11->setText(ckc[22]); ui->minKey11->setText(ckc[23]);
  ui->silence->setText(ckc[24]);

  ui->filterFileExtensions->setText(p.getFilterFileExtensions().join(","));

  // enable/disable fields as necessary
  metadataDelimiterEnabled();
  applyFileExtensionFilterEnabled();

  //relative sizing on Mac/Linux only
#ifndef Q_OS_WIN
  QFont smallerFont;
  smallerFont.setPointSize(smallerFont.pointSize() - 2);
  for (int i=0; i<ui->gridLayoutKeyCodes->count(); i++) {
    if (!ui->gridLayoutKeyCodes->itemAt(i)->isEmpty() && ui->gridLayoutKeyCodes->itemAt(i)->widget()->inherits("QLabel")) {
      ui->gridLayoutKeyCodes->itemAt(i)->widget()->setFont(smallerFont);
    }
  }
#endif

  // validation of file extension filtering
  QRegExp regex("^([a-z0-9]+,?)*$");
  QValidator *validator = new QRegExpValidator(regex, this);
  ui->filterFileExtensions->setValidator(validator);
}

PrefsDialog::~PrefsDialog() {
  delete ui;
}

void PrefsDialog::on_savePrefsButton_clicked() {
  Preferences p;
  p.setWriteToFilesAutomatically(ui->writeToFilesAutomatically->isChecked());
  p.setParallelBatchJobs(ui->parallelBatchJobs->isChecked());
  p.setApplyFileExtensionFilter(ui->applyFileExtensionFilter->isChecked());
  p.setMetadataFormat(listMetadataFormat[ui->tagFormat->currentIndex()]);
  p.setMetadataWriteTitle(listMetadataWrite[ui->metadataWriteTitle->currentIndex()]);
  p.setMetadataWriteArtist(listMetadataWrite[ui->metadataWriteArtist->currentIndex()]);
  p.setMetadataWriteAlbum(listMetadataWrite[ui->metadataWriteAlbum->currentIndex()]);
  p.setMetadataWriteComment(listMetadataWrite[ui->metadataWriteComment->currentIndex()]);
  p.setMetadataWriteGrouping(listMetadataWrite[ui->metadataWriteGrouping->currentIndex()]);
  p.setMetadataWriteKey(listMetadataWriteKey[ui->metadataWriteKey->currentIndex()]);
  p.setMetadataWriteFilename(listMetadataWrite[ui->metadataWriteFilename->currentIndex()]);
  p.setMetadataDelimiter(ui->metadataDelimiter->text());
  p.setSkipFilesWithExistingTags(ui->skipFilesWithExistingTags->isChecked());
  p.setMaxDuration(ui->maxDuration->value());
  p.setITunesLibraryPath(ui->iTunesLibraryPath->text());
  p.setTraktorLibraryPath(ui->traktorLibraryPath->text());
  p.setSeratoLibraryPath(ui->seratoLibraryPath->text());

  QStringList ckc;
  ckc << ui->majKey0->text() << ui->minKey0->text();
  ckc << ui->majKey1->text() << ui->minKey1->text();
  ckc << ui->majKey2->text() << ui->minKey2->text();
  ckc << ui->majKey3->text() << ui->minKey3->text();
  ckc << ui->majKey4->text() << ui->minKey4->text();
  ckc << ui->majKey5->text() << ui->minKey5->text();
  ckc << ui->majKey6->text() << ui->minKey6->text();
  ckc << ui->majKey7->text() << ui->minKey7->text();
  ckc << ui->majKey8->text() << ui->minKey8->text();
  ckc << ui->majKey9->text() << ui->minKey9->text();
  ckc << ui->majKey10->text() << ui->minKey10->text();
  ckc << ui->majKey11->text() << ui->minKey11->text();
  ckc << ui->silence->text();
  p.setCustomKeyCodes(ckc);

  p.setFilterFileExtensions(ui->filterFileExtensions->text().split(","));

  // save to QSettings and close window
  p.save();
  this->close();
}

void PrefsDialog::on_cancelButton_clicked() {
  this->close();
}

void PrefsDialog::metadataDelimiterEnabled() {
  QList<int> indices;
  indices << ui->metadataWriteTitle->currentIndex();
  indices << ui->metadataWriteArtist->currentIndex();
  indices << ui->metadataWriteAlbum->currentIndex();
  indices << ui->metadataWriteComment->currentIndex();
  indices << ui->metadataWriteGrouping->currentIndex();
  indices << ui->metadataWriteFilename->currentIndex();
  for (int i = 0; i < indices.size(); i++) {
    metadata_write_t chk = listMetadataWrite[indices[i]];
    if (chk == METADATA_WRITE_PREPEND || chk == METADATA_WRITE_APPEND) {
      ui->metadataDelimiter->setEnabled(true);
      return;
    }
  }
  ui->metadataDelimiter->setEnabled(false);
}

void PrefsDialog::applyFileExtensionFilterEnabled() {
  ui->filterFileExtensions->setEnabled(ui->applyFileExtensionFilter->isChecked());
}

void PrefsDialog::on_metadataWriteTitle_currentIndexChanged(int /*index*/) {
  metadataDelimiterEnabled();
}

void PrefsDialog::on_metadataWriteArtist_currentIndexChanged(int /*index*/) {
  metadataDelimiterEnabled();
}

void PrefsDialog::on_metadataWriteAlbum_currentIndexChanged(int /*index*/) {
  metadataDelimiterEnabled();
}

void PrefsDialog::on_metadataWriteComment_currentIndexChanged(int /*index*/) {
  metadataDelimiterEnabled();
}

void PrefsDialog::on_metadataWriteGrouping_currentIndexChanged(int /*index*/) {
  metadataDelimiterEnabled();
}

void PrefsDialog::on_metadataWriteKey_currentIndexChanged(int /*index*/) {
  metadataDelimiterEnabled();
}

void PrefsDialog::on_metadataWriteFilename_currentIndexChanged(int /*index*/) {
  metadataDelimiterEnabled();
}

void PrefsDialog::on_applyFileExtensionFilter_stateChanged(int /*state*/) {
  applyFileExtensionFilterEnabled();
}

void PrefsDialog::on_findITunesLibraryButton_clicked() {
  QString initDir;
#ifdef Q_OS_WIN
  initDir = QDir::homePath() + "/My Music";
#else
  initDir = QDir::homePath() + "/Music/iTunes";
#endif
  //: File browse dialog launched from the Preferences window
  QString iTunesLibraryPath = QFileDialog::getOpenFileName(
                                this,
                                //: Title of file browse dialog launched from the Preferences window
                                tr("Choose iTunes Library XML"),
                                initDir,
                                //: Filetype in file browse dialog launched from the Preferences window
                                tr("iTunes XML (*.xml)")
                                );
  if (iTunesLibraryPath.isEmpty()) {
    return;
  }
  ui->iTunesLibraryPath->setText(iTunesLibraryPath);
}

void PrefsDialog::on_findTraktorLibraryButton_clicked() {
  QString initDir;
#ifdef Q_OS_WIN
  initDir = QDir::homePath() + "/My Music";
#else
  initDir = QDir::homePath() + "/Documents/Native Instruments/Traktor 2.1.2";
#endif
  QString traktorLibraryPath = QFileDialog::getOpenFileName(
                                 this,
                                 //: Title of file browse dialog launched from the Preferences window
                                 tr("Choose Traktor Library NML"),
                                 initDir,
                                 //: Filetype in file browse dialog launched from the Preferences window
                                 tr("Traktor NML (*.nml)")
                                 );
  if (traktorLibraryPath.isEmpty()) {
    return;
  }
  ui->traktorLibraryPath->setText(traktorLibraryPath);
}

void PrefsDialog::on_findSeratoLibraryButton_clicked() {
  QString initDir;
#ifdef Q_OS_WIN
  initDir = QDir::homePath() + "/My Music";
#else
  initDir = QDir::homePath() + "/Music/_Serato_";
#endif
  QString seratoLibraryPath = QFileDialog::getOpenFileName(
                                this,
                                //: Title of file browse dialog launched from the Preferences window
                                tr("Choose Serato database"),
                                initDir,
                                //: Filetype in file browse dialog launched from the Preferences window
                                tr("Serato database (*)")
                                );
  if (seratoLibraryPath.isEmpty()) {
    return;
  }
  ui->seratoLibraryPath->setText(seratoLibraryPath);
}
