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

#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include <QtWidgets/QDialog>
#include <QString>
#include <QtWidgets/QFileDialog>

#include "preferences.h"

namespace Ui {
  class PrefsDialog;
}

class PrefsDialog : public QDialog {
  Q_OBJECT
public:
  explicit PrefsDialog(QWidget* parent = 0);
  ~PrefsDialog();
private:
  // map combobox indices to useful values
  QList<metadata_format_t>               listMetadataFormat;
  QList<metadata_write_t>                listMetadataWrite;
  QList<metadata_write_t>                listMetadataWriteKey;
  // altering state on field changes
  void metadataDelimiterEnabled();
  void applyFileExtensionFilterEnabled();
private slots:
  void on_savePrefsButton_clicked();
  void on_cancelButton_clicked();
  void on_findITunesLibraryButton_clicked();
  void on_findTraktorLibraryButton_clicked();
  void on_findSeratoLibraryButton_clicked();
  void on_metadataWriteTitle_currentIndexChanged(int index);
  void on_metadataWriteArtist_currentIndexChanged(int index);
  void on_metadataWriteAlbum_currentIndexChanged(int index);
  void on_metadataWriteComment_currentIndexChanged(int index);
  void on_metadataWriteGrouping_currentIndexChanged(int index);
  void on_metadataWriteKey_currentIndexChanged(int index);
  void on_metadataWriteFilename_currentIndexChanged(int index);
  void on_applyFileExtensionFilter_stateChanged(int state);
private:
  Ui::PrefsDialog *ui;
};

#endif // PREFSDIALOG_H
