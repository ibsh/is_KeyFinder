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

void MainMenuHandler::new_Batch_Window(){
  BatchWindow* newWin = new BatchWindow(this,0);
  newWin->setMenuBar(newMenuBar());
  newWin->show();
}

void MainMenuHandler::new_Detail_Window(QString path){
  DetailWindow* newWin = new DetailWindow(0,path);
  std::cerr << newWin->menuBar() << std::endl;
  newWin->setMenuBar(newMenuBar());
  newWin->show();
}

void MainMenuHandler::close_Window(){
  QApplication::activeWindow()->close();
}

QMenuBar* MainMenuHandler::newMenuBar(){
  QMenuBar* menuBar = new QMenuBar(0);
  QMenu* menuFile = menuBar->addMenu("File");
  menuFile->addAction("New Detail Window",this,SLOT(new_Detail_Window()),QKeySequence("Ctrl+N"));
  menuFile->addAction("New Batch Window",this,SLOT(new_Batch_Window()),QKeySequence("Ctrl+Shift+N"));
  menuFile->addSeparator();
  menuFile->addAction("Close Window",this,SLOT(close_Window()),QKeySequence("Ctrl+W"));
<<<<<<< HEAD
  QMenu* menuEdit = menuBar->addMenu("Edit");
  menuEdit->addAction("Preferences",this,SLOT(preferences()));
  QMenu* menuHelp = menuBar->addMenu("Help");
  menuHelp->addAction("About",this,SLOT(about()));
=======
  #ifdef Q_OS_MAC
    menuFile->addAction("Preferences",this,SLOT(preferences()));
    menuFile->addAction("About",this,SLOT(about()));
  #endif
  #ifdef Q_OS_WIN
    QMenu* menuEdit = menuBar->addMenu("Edit");
    menuEdit->addAction("Preferences",this,SLOT(preferences()));
    QMenu* menuHelp = menuBar->addMenu("Help");
    menuHelp->addAction("About",this,SLOT(about()));
  #endif
>>>>>>> v1.1.0
  return menuBar;
}
