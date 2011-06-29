#include "spectrumanalyserfactory.h"

SpectrumAnalyserFactory* SpectrumAnalyserFactory::inst = NULL;

SpectrumAnalyserFactory* SpectrumAnalyserFactory::getInstance(){
	std::cerr << "GET INSTANCE" << std::endl;
	if(inst == NULL){
		std::cerr << "INIT NEW INSTANCE" << std::endl;
		inst = new SpectrumAnalyserFactory();
	}
	return inst;
}

SpectrumAnalyserFactory::SpectrumAnalyserFactory(){
	std::cerr << "CONSTRUCTOR" << std::endl;
	analysers = std::vector<SpectrumAnalyser*>(0);
	types = std::vector<char>(0);
	fpps = std::vector<char>(0);
	framerates = std::vector<int>(0);
	windows = std::vector<char>(0);
}

SpectrumAnalyserFactory::~SpectrumAnalyserFactory(){
	std::cerr << "DESTRUCTOR" << std::endl;
	for(int i=0; i<analysers.size(); i++){
		std::cerr << "DEL " << i << std::endl;
		delete analysers[i];
	}
	inst = NULL;
}

SpectrumAnalyser* SpectrumAnalyserFactory::getSpectrumAnalyser(int frameRate, const Preferences& prefs){
	std::cerr << "GET SA FROM " << analysers.size() << std::endl;
	char chkType = prefs.getSpectrumAnalyser();
	char chkFpp = prefs.getFftPostProcessor();
	char chkWin = prefs.getTemporalWindow();
	for(int i=0; i<analysers.size(); i++){
		std::cerr << "CHECK " << i << std::endl;
		if(framerates[i] == frameRate 	&& types[i] == chkType && fpps[i] == chkFpp && windows[i] == chkWin){
			std::cerr << "RETURN " << i << std::endl;
			return analysers[i];
		}
	}
	std::cerr << "MAKE NEW" << std::endl;
	framerates.push_back(frameRate);
	types.push_back(chkType);
	fpps.push_back(chkFpp);
	windows.push_back(chkWin);
	if(chkType == 'g')
		analysers.push_back(new GoertzelAnalyser(frameRate,prefs));
	else
		analysers.push_back(new FftwAnalyser(frameRate,prefs));
	std::cerr << "RETURN " << analysers.size() - 1 << std::endl;
	return analysers[analysers.size()-1];
}
