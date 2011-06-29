#ifndef SPECTRUMANALYSERFACTORY_H
#define SPECTRUMANALYSERFACTORY_H

#include "spectrumanalyser.h"
#include "goertzelanalyser.h"
#include "fftwanalyser.h"
#include "preferences.h"

#include <vector>
#include <iostream>

// Singleton. It caches all SAs generated in a session to cut down on prep time.

class SpectrumAnalyserFactory{
public:
	SpectrumAnalyser* getSpectrumAnalyser(int, const Preferences&);
	static SpectrumAnalyserFactory* getInstance();
	~SpectrumAnalyserFactory();
private:
	SpectrumAnalyserFactory();
	static SpectrumAnalyserFactory* inst;
	std::vector<SpectrumAnalyser*> analysers;
	std::vector<char> types;
	std::vector<char> fpps;
	std::vector<int> framerates;
	std::vector<char> windows;
};

#endif // SPECTRUMANALYSERFACTORY_H
