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

MainMenuHandler::MainMenuHandler(QObject *parent) : QObject(parent) {}

void MainMenuHandler::about() {
  AboutDialog* about = new AboutDialog(NULL);
  about->show();
}

void MainMenuHandler::preferences() {
  PrefsDialog* prefs = new PrefsDialog(NULL);
  prefs->show();
}

void MainMenuHandler::new_Batch_Window(bool firstWindow) {
  BatchWindow* newWin = new BatchWindow(NULL, this);
  newWin->setMenuBar(newMenuBar());
  newWin->show();
  if (firstWindow)
    newWin->checkForNewVersion();
}

void MainMenuHandler::new_Detail_Window(QString path) {
  DetailWindow* newWin = new DetailWindow(NULL, path);
  newWin->setMenuBar(newMenuBar());
  newWin->show();
}

void MainMenuHandler::close_Window() {
  QApplication::activeWindow()->close();
}

QMenuBar* MainMenuHandler::newMenuBar() {
  QMenuBar* menuBar = new QMenuBar(NULL);
  QMenu* menuFile = menuBar->addMenu("File");
  //: An action on the menu bar
  menuFile->addAction(tr("New Detail Window"), this, SLOT(new_Detail_Window()), QKeySequence::New);
  //: An action on the menu bar
  menuFile->addAction(tr("New Batch Window"), this, SLOT(new_Batch_Window()), QKeySequence("Ctrl+Shift+N"));
  menuFile->addSeparator();
  //: An action on the menu bar
  menuFile->addAction(tr("Close Window"), this, SLOT(close_Window()), QKeySequence::Close);
#ifdef Q_OS_WIN
  QMenu* menuEdit = menuBar->addMenu("Edit");
  menuEdit->addAction(GuiStrings::getInstance()->preferences(), this, SLOT(preferences()));
  QMenu* menuHelp = menuBar->addMenu("Help");
  menuHelp->addAction(GuiStrings::getInstance()->about().arg(GuiStrings::getInstance()->appName()), this, SLOT(about()));
#else
  menuFile->addAction(GuiStrings::getInstance()->preferences(), this, SLOT(preferences()), QKeySequence::Preferences);
  menuFile->addAction(GuiStrings::getInstance()->about(), this, SLOT(about()));
#endif
  return menuBar;
}
