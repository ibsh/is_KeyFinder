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

#ifndef SPECTRUMANALYSERFACTORY_H
#define SPECTRUMANALYSERFACTORY_H

#include <QMutexLocker>

#include "spectrumanalyser.h"
#include "goertzelanalyser.h"
#include "fftwanalyser.h"
#include "preferences.h"

#include <vector>

// Keeps a reference to a spectrum analyser with distinguishing information
class SpectrumAnalyserWrapper{
public:
	SpectrumAnalyserWrapper(char,char,int,char,SpectrumAnalyser*);
	~SpectrumAnalyserWrapper();
	char getType() const;
	char getFftPostProcessor() const;
	int getFramerate() const;
	char getTemporalWindow() const;
	SpectrumAnalyser* getSpectrumAnalyser() const;
private:
	char type;
	char fftpp;
	int frate;
	char window;
	SpectrumAnalyser* sa;
};

// Singleton. It holds all analysers generated in a session, to cut down on prep time.
class SpectrumAnalyserFactory{
public:
	SpectrumAnalyser* getSpectrumAnalyser(int, const Preferences&);
	static SpectrumAnalyserFactory* getInstance();
	~SpectrumAnalyserFactory();
private:
	mutable QMutex mutex; // used to make getSpectrumAnalyser thread-safe
	SpectrumAnalyserFactory();
	static SpectrumAnalyserFactory* inst;
	std::vector<SpectrumAnalyserWrapper*> analysers;
};

#endif // SPECTRUMANALYSERFACTORY_H
