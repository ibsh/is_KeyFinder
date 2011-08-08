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

#ifndef GOERTZELANALYSER_H
#define GOERTZELANALYSER_H

#include "spectrumanalyser.h"
#include "preferences.h"
#include "windowfunctions.h"
#include <math.h>
#include <vector>
#include <stdlib.h>

class GoertzelAnalyser : public SpectrumAnalyser{
public:
	GoertzelAnalyser(int, const Preferences&);
	~GoertzelAnalyser();
	virtual Chromagram* chromagram(AudioBuffer*);
private:
	class Goertzel;
	std::vector<Goertzel*> goertzels;
};

class GoertzelAnalyser::Goertzel{
public:
	Goertzel(float,int,int,char);
	void processSample(float);
	float getRelativeMagnitude() const;
	bool samplesWanted() const;
	void reset();
	int getN() const;
private:
	float frequency;
	float coeff;
	float sine;
	float cosine;
	float Q1, Q2;
	int N;
	int k;
	int samplesProcessed;
	std::vector<float> window;
};

#endif
