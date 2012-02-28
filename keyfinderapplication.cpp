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
