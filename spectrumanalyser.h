#ifndef SPECTRUMANALYSER_H
#define SPECTRUMANALYSER_H

#include <QMutexLocker>

#include "chromagram.h"
#include "audiobuffer.h"
#include "preferences.h"

class SpectrumAnalyser{
public:
	SpectrumAnalyser(int, const Preferences&);
	virtual Chromagram* chromagram(AudioBuffer*) = 0;
protected:
	mutable QMutex mutex; // used to make chromagram functions thread-safe
	int bins;
	int hopSize;
	int frameRate;
};

#endif
