#include "spectrumanalyserfactory.h"

SpectrumAnalyserFactory* SpectrumAnalyserFactory::getInstance(){

}

SpectrumAnalyser* SpectrumAnalyserFactory::getSpectrumAnalyser(int frameRate,const Preferences& prefs){

}

SpectrumAnalyserFactory::SpectrumAnalyserFactory(){
	cache = std::vector<SpectrumAnalyserCacheEntry>(0);
}

SpectrumAnalyserFactory::SpectrumAnalyserCacheEntry::SpectrumAnalyserCacheEntry(){
	sa = NULL;
	frameRate = 0;
	whichSa = ' ';
	whichFftPp = ' ';
	whichTmpWin = ' ';
}

SpectrumAnalyserFactory::SpectrumAnalyserCacheEntry::~SpectrumAnalyserCacheEntry(){
	delete sa;
}
