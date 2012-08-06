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
  this->setWindowTitle(tr("About %1").arg(GuiStrings::getInstance()->appName()));
  this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

  QString br = "\n";

  ui->label_1_vn->setText(GuiStrings::getInstance()->appName() + br + "v" + QString::number(VERSION_MAJOR) + "." + QString::number(VERSION_MINOR));
  ui->label_2_cp->setText(tr("Copyright %1").arg("2011-" + QString::number(QDate::currentDate().year())) + br + "Ibrahim Sha'ath");
  ui->label_4_includes->setText(tr("%1 includes library code from:").arg(GuiStrings::getInstance()->appName()));

  QString libs;
  libs += "Qt (qt.nokia.com)" + br;
  libs += "FFTW (www.fftw.org)" + br;
  libs += "LibAV (www.libav.org)" + br;
  libs += "Secret Rabbit Code (www.mega-nerd.com/SRC)" + br;
  libs += "TagLib (developer.kde.org/~wheeler/taglib.html)" + br;
  libs += "XQilla (xqilla.sourceforge.net)" + br;
  libs += "Xerces-C++ (xerces.apache.org)";
  ui->label_5_libs->setText(libs);

  //relative sizing on Mac/Linux only
#ifndef Q_OS_WIN
  QFont smallerFont;
  smallerFont.setPointSize(smallerFont.pointSize() - 2);
  ui->label_3_gpl->setFont(smallerFont);
  ui->label_4_includes->setFont(smallerFont);
  ui->label_5_libs->setFont(smallerFont);
#endif
}

AboutDialog::~AboutDialog(){
  delete ui;
}
