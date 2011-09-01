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

#ifndef CHROMAGRAM_H
#define CHROMAGRAM_H

#include "preferences.h"
#include <vector>
#include <math.h>
#include <stdlib.h>

class Chromagram{
public:
	Chromagram(int h = 0,int b = 0);
	void setMagnitude(int,int,float);
	float getMagnitude(int,int) const;
	int getHops() const;
	int getBins() const;
	void reduceToOneOctave(const Preferences&);
	void reduceTuningBins(const Preferences&);
private:
	int hops;
	int bins;
	std::vector<std::vector<float> > chromaData;
	void tuningHarte(const Preferences&);
	void tuningBinAdaptive(const Preferences&);
};

#endif
