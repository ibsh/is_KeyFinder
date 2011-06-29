#ifndef SPECTRUMANALYSERFACTORY_H
#define SPECTRUMANALYSERFACTORY_H

#include "spectrumanalyser.h"
#include "preferences.h"
#include <vector>

// Factory and Singleton. It caches all SAs generated in this session.

class SpectrumAnalyserFactory{
public:
	SpectrumAnalyser* getSpectrumAnalyser(int,const Preferences&);
	SpectrumAnalyserFactory* getInstance();
private:
	SpectrumAnalyserFactory();
	static SpectrumAnalyserFactory instance;
	class SpectrumAnalyserCacheEntry;
	std::vector<SpectrumAnalyserCacheEntry> cache;
};

class SpectrumAnalyserFactory::SpectrumAnalyserCacheEntry{
public:
	SpectrumAnalyserCacheEntry();
	~SpectrumAnalyserCacheEntry();
	SpectrumAnalyser* sa;
	int frameRate;
	char whichSa;
	char whichFftPp;
	char whichTmpWin;
};

#endif // SPECTRUMANALYSERFACTORY_H
