/*************************************************************************

  Copyright 2011-2015 Ibrahim Sha'ath

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

#ifndef KEYFINDERRESULTWRAPPER_H
#define KEYFINDERRESULTWRAPPER_H

#include <QString>
#include <keyfinder/keyfinder.h>

class KeyFinderResultWrapper {
public:
  KeyFinder::key_t core;
  KeyFinder::Chromagram fullChromagram;
  int batchRow;
  QString errorMessage;
};

#endif // KEYFINDERRESULTWRAPPER_H
