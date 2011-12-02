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

#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include <QDialog>
#include <QString>
#include <QSettings>

#include "preferences.h"

namespace Ui {
	class PrefsDialog;
}

class PrefsDialog : public QDialog{
Q_OBJECT
public:
	explicit PrefsDialog(QWidget *parent = 0);
	~PrefsDialog();
private:
	void tuningEnabled();
	void binAdaptiveTuningEnabled();
	void hcdfEnabled();
	void customProfileEnabled();
	QString temporalWindowComboIndex;
	QString hcdfComboIndex;
	QString similarityMeasureComboIndex;
  QString tagFormatComboIndex;
  QString tagFieldComboIndex;
private slots:
	void on_savePrefsButton_clicked();
	void on_cancelButton_clicked();
	void on_bps_valueChanged(int arg1);
	void on_hcdf_currentIndexChanged(int index);
	void on_toneProfile_currentIndexChanged(int index);
	void on_tuningMethod_currentIndexChanged(int index);
private:
	Ui::PrefsDialog *ui;
};

#endif // PREFSDIALOG_H
