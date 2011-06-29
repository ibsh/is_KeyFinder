#include "spectrumanalyser.h"

SpectrumAnalyser::SpectrumAnalyser(int f, const Preferences& prefs){
	bins = prefs.getOctaves() * prefs.getBpo();
	hopSize = prefs.getHopSize();
	frameRate = f;
}
