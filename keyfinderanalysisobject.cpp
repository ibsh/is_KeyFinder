#include "keyfinderanalysisobject.h"

KeyFinderAnalysisObject::KeyFinderAnalysisObject(const QString& f, const Preferences& p, int r){
  filePath = f;
  prefs = p;
  batchRow = r;
}
