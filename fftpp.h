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

#ifndef FFTPOSTPROCESSOR_H
#define FFTPOSTPROCESSOR_H

#include "preferences.h"
#include <fftw3.h>

class FftPostProcessor{
public:
  static FftPostProcessor* getFftPostProcessor(int, const Preferences&);
	FftPostProcessor(int, const Preferences&);
	virtual std::vector<float> chromaVector(fftw_complex*) const = 0;
	virtual void printKernel() const = 0;
protected:
	int bins;
	int fftFrameSize;
	int frameRate;
};

#endif
