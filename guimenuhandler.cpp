/*************************************************************************

  Copyright 2011-2013 Ibrahim Sha'ath

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

#include "guimenuhandler.h"

MainMenuHandler::MainMenuHandler(QObject *parent) : QObject(parent){}

void MainMenuHandler::about(){
  AboutDialog* about = new AboutDialog(0);
  about->show();
}

void MainMenuHandler::preferences(){
  PrefsDialog* prefs = new PrefsDialog(0);
  prefs->show();
}

void MainMenuHandler::new_Batch_Window(bool firstWindow){
  BatchWindow* newWin = new BatchWindow(this, 0);
  newWin->setMenuBar(newMenuBar());
  newWin->show();
  if(firstWindow)
    newWin->checkForNewVersion();
}

void MainMenuHandler::new_Detail_Window(QString path){
  DetailWindow* newWin = new DetailWindow(0, path);
  newWin->setMenuBar(newMenuBar());
  newWin->show();
}

void MainMenuHandler::close_Window(){
  QApplication::activeWindow()->close();
}

QMenuBar* MainMenuHandler::newMenuBar(){
  QMenuBar* menuBar = new QMenuBar(0);
  QMenu* menuFile = menuBar->addMenu("File");
  //: An action on the menu bar
  menuFile->addAction(tr("New Detail Window"), this, SLOT(new_Detail_Window()), QKeySequence("Ctrl+N"));
  //: An action on the menu bar
  menuFile->addAction(tr("New Batch Window"), this, SLOT(new_Batch_Window()), QKeySequence("Ctrl+Shift+N"));
  menuFile->addSeparator();
  //: An action on the menu bar
  menuFile->addAction(tr("Close Window"), this, SLOT(close_Window()), QKeySequence("Ctrl+W"));
#ifdef Q_OS_WIN
  QMenu* menuEdit = menuBar->addMenu("Edit");
  //: An action on the menu bar
  menuEdit->addAction(tr("Preferences"), this, SLOT(preferences()));
  QMenu* menuHelp = menuBar->addMenu("Help");
  //: An action on the menu bar
  menuHelp->addAction(tr("About"), this, SLOT(about()));
#else
  menuFile->addAction(tr("Preferences"), this, SLOT(preferences()));
  menuFile->addAction(tr("About"), this, SLOT(about()));
#endif
  return menuBar;
}
