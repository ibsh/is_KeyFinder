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

#ifndef SPECTRUMANALYSER_H
#define SPECTRUMANALYSER_H

#include <QMutexLocker>

#include "chromagram.h"
#include "audiostream.h"
#include "preferences.h"

class SpectrumAnalyser{
public:
  SpectrumAnalyser(int, const Preferences&);
  virtual Chromagram* chromagram(AudioStream*) = 0;
  virtual ~SpectrumAnalyser();
protected:
  mutable QMutex mutex; // used to make chromagram functions thread-safe
  int bins;
  int hopSize;
  int frameRate;
};

#endif
