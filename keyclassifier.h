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

#ifndef KEYCLASSIFIER_H
#define KEYCLASSIFIER_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include "toneprofiles.h"
#include "preferences.h"

class KeyClassifier{
public:
  KeyClassifier(const Preferences&);
  ~KeyClassifier();
  int classify(const std::vector<double>&);
private:
  ToneProfile* major;
  ToneProfile* minor;
  ToneProfile* silence;
  char similarityMeasure;
};

#endif
