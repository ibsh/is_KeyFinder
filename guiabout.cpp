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

#include "guiabout.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent): QDialog(parent),ui(new Ui::AboutDialog){
	ui->setupUi(this);
  this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

  ui->label_1_vn->setText("KeyFinder\nv1.10");

  ui->label_2_cp->setText("Copyright 2011-" + QString::number(QDate::currentDate().year()) + "\nIbrahim Sha'ath");

  //relative sizing on Mac only
  #ifdef Q_OS_MAC
    QFont smallerFont;
    smallerFont.setPointSize(smallerFont.pointSize() - 2);
    ui->label_3_gpl->setFont(smallerFont);
  #endif
}

AboutDialog::~AboutDialog(){
	delete ui;
}
