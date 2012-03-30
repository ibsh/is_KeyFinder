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

#include "keyfinderapplication.h"

KeyFinderApplication::KeyFinderApplication(int& a, char**& b) : QApplication(a,b){ }

bool KeyFinderApplication::event(QEvent* event){
  switch (event->type()) {
  case QEvent::FileOpen:
    loadFile(static_cast<QFileOpenEvent *>(event)->url());
    return true;
  default:
    return QApplication::event(event);
  }
}

void KeyFinderApplication::loadFile(const QUrl& fileUrl){
  QList<QUrl> urls;
  urls.push_back(fileUrl);
  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    if(QString(widget->metaObject()->className()) == "BatchWindow"){
      if (static_cast<BatchWindow*>(widget)->receiveUrls(urls) == false)
        return;
    }
  }
  QApplication::beep();
}
