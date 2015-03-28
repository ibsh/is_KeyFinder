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

#ifndef MAINMENUHANDLER_H
#define MAINMENUHANDLER_H

// forward declaration for circular dependency
class BatchWindow;

#include <QtGui>
#include <QtWidgets>
#include <QObject>

#include "guibatch.h"
#include "guiabout.h"
#include "guiprefs.h"

class MainMenuHandler : public QObject {
  Q_OBJECT
public:
  explicit MainMenuHandler(QObject *parent = 0);
public slots:
  void about();
  void preferences();
  void newBatchWindow(bool firstWindow = false);
  void close_Window();
  QMenuBar* newMenuBar();
};

#endif // MAINMENUHANDLER_H
