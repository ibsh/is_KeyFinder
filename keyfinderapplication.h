#ifndef KEYFINDERAPPLICATION_H
#define KEYFINDERAPPLICATION_H

#include <QApplication>
#include <QFileOpenEvent>
#include <QWidget>
#include <QDropEvent>
#include <QUrl>

#include "guibatch.h"

class KeyFinderApplication : public QApplication{
  Q_OBJECT
public:
  KeyFinderApplication(int&,char**&);
protected:
  bool event(QEvent*);
private:
  void loadFile(const QUrl&);
};

#endif // KEYFINDERAPPLICATION_H
