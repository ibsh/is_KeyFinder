#include "spectrumanalyserfactory.h"

SpectrumAnalyserFactory* SpectrumAnalyserFactory::inst = NULL;

SpectrumAnalyserFactory* SpectrumAnalyserFactory::getInstance(){
	if(inst == NULL){
		inst = new SpectrumAnalyserFactory();
	}
	return inst;
}

SpectrumAnalyserFactory::SpectrumAnalyserFactory(){
	analysers = std::vector<SpectrumAnalyser*>(0);
	types = std::vector<char>(0);
	fpps = std::vector<char>(0);
	framerates = std::vector<int>(0);
	windows = std::vector<char>(0);
}

SpectrumAnalyserFactory::~SpectrumAnalyserFactory(){
	for(int i=0; i<analysers.size(); i++){
		delete analysers[i];
	}
	inst = NULL;
}

SpectrumAnalyser* SpectrumAnalyserFactory::getSpectrumAnalyser(int frameRate, const Preferences& prefs){
	QMutexLocker locker(&mutex); // This function should be accessed by only one thread at a time
	char chkType = prefs.getSpectrumAnalyser();
	char chkFpp = prefs.getFftPostProcessor();
	char chkWin = prefs.getTemporalWindow();
	for(int i=0; i<analysers.size(); i++)
		if(framerates[i] == frameRate 	&& types[i] == chkType && fpps[i] == chkFpp && windows[i] == chkWin)
			return analysers[i];
	framerates.push_back(frameRate);
	types.push_back(chkType);
	fpps.push_back(chkFpp);
	windows.push_back(chkWin);
	if(chkType == 'g')
		analysers.push_back(new GoertzelAnalyser(frameRate,prefs));
	else
		analysers.push_back(new FftwAnalyser(frameRate,prefs));
	return analysers[analysers.size()-1];
}
