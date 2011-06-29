#ifndef SPECTRUMANALYSER_H
#define SPECTRUMANALYSER_H

#include "chromagram.h"
#include "audiobuffer.h"
#include "preferences.h"

class SpectrumAnalyser{
public:
	SpectrumAnalyser(int, const Preferences&);
	virtual Chromagram* chromagram(AudioBuffer*) = 0;
protected:
	int bins;
	int hopSize;
	int frameRate;
};

#endif
