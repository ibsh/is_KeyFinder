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

#ifndef HCDFHARTE_H
#define HCDFHARTE_H

#include "hcdf.h"

class HarteHcdf : public Segmentation{
public:
  HarteHcdf();
  virtual std::vector<double> getRateOfChange(Chromagram*, const Preferences&);
  virtual std::vector<int> getSegments(const std::vector<double>&, const Preferences&);
private:
  double phi(int, int);
  double pi;
};

#endif // HCDFHARTE_H
