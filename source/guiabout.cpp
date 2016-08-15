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

#include "guiabout.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent): QDialog(parent), ui(new Ui::AboutDialog) {
  ui->setupUi(this);
  //: The title of the About window; includes the app name at %1
  this->setWindowTitle(tr("About %1").arg(GuiStrings::getInstance()->appName()));
  this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

  QString br = "\n";

  ui->label_1_vn->setText(GuiStrings::getInstance()->appName() + br + "v" + QString::number(VERSION_MAJOR) + "." + QString::number(VERSION_MINOR));
  //: The copyright notice in the About window; includes the year and author name at %1
  ui->label_2_cp->setText(tr("Copyright %1").arg("2011-" + QString::number(QDate::currentDate().year())) + br + "Ibrahim Sha'ath");

  QString mid;

  //: Icon citation on the About window; includes the app name at %1 and the designer name at %2
  mid += tr("The %1 icon was designed by %2.").arg(GuiStrings::getInstance()->appName()).arg("Ollin Boer Bohan");

  mid += br;
  mid += br;

  //: Library list header on the About window; includes the app name at %1
  mid += tr("%1 includes library code from:").arg(GuiStrings::getInstance()->appName());

  ui->label_5_includes->setText(mid);

  QString libs;
  libs += "Qt (qt-project.org)" + br;
  libs += "FFTW (www.fftw.org)" + br;
  libs += "LibAV (www.libav.org)" + br;
  libs += "TagLib (developer.kde.org/~wheeler/taglib.html)" + br;
  ui->label_6_libs->setText(libs);

  //relative sizing on Mac/Linux only
#ifndef Q_OS_WIN
  QFont smallerFont;
  smallerFont.setPointSize(smallerFont.pointSize() - 2);
  ui->label_4_gpl->setFont(smallerFont);
  ui->label_5_includes->setFont(smallerFont);
  ui->label_6_libs->setFont(smallerFont);
#endif
}

AboutDialog::~AboutDialog() {
  delete ui;
}
