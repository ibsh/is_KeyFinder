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

#ifndef KEYFINDERMODEL_H
#define KEYFINDERMODEL_H

#include <QFile>
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

#include "asynckeyobject.h"
#include "asynckeyresult.h"

// trying this as a global function rather than an object...
KeyDetectionResult keyDetectionProcess(const KeyDetectionObject&);

#endif // KEYFINDERMODEL_H
