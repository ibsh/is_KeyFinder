#ifndef KEYFINDERMODEL_H
#define KEYFINDERMODEL_H

#include <QString>

#include <vector>

#include "preferences.h"
#include "exception.h"
#include "audiostream.h"
#include "decoder.h"
#include "downsampler.h"
#include "chromagram.h"
#include "spectrumanalyserfactory.h"
#include "hcdf.h"
#include "keyclassifier.h"

#include "keyfinderanalysisobject.h"
#include "keyfinderresultset.h"

// trying this as a global function rather than an object...
KeyFinderResultSet keyFinderProcessObject(const KeyFinderAnalysisObject&);

#endif // KEYFINDERMODEL_H
