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

#include "spectrumanalyserfactory.h"

SpectrumAnalyserWrapper::SpectrumAnalyserWrapper(int f, SpectrumAnalyser* s){
	frate = f;
	sa = s;
	prefs = Preferences();
}

SpectrumAnalyserWrapper::~SpectrumAnalyserWrapper(){
	delete sa;
}

SpectrumAnalyser* SpectrumAnalyserWrapper::getSpectrumAnalyser() const{
	return sa;
}

Preferences SpectrumAnalyserWrapper::chkPrefs() const{
	return prefs;
}

int SpectrumAnalyserWrapper::chkFrameRate() const{
	return frate;
}

SpectrumAnalyserFactory* SpectrumAnalyserFactory::inst = NULL;

SpectrumAnalyserFactory* SpectrumAnalyserFactory::getInstance(){
	if(inst == NULL)
		inst = new SpectrumAnalyserFactory();
	return inst;
}

SpectrumAnalyserFactory::SpectrumAnalyserFactory(){
	analysers = std::vector<SpectrumAnalyserWrapper*>(0);
}

SpectrumAnalyserFactory::~SpectrumAnalyserFactory(){
	for(int i=0; i<(signed)analysers.size(); i++)
		delete analysers[i];
	inst = NULL;
}

SpectrumAnalyser* SpectrumAnalyserFactory::getSpectrumAnalyser(int frameRate, const Preferences& prefs){
	QMutexLocker locker(&mutex); // This function should be accessed by only one thread at a time
	for(int i=0; i<(signed)analysers.size(); i++){
		bool foundMatch = false;
		if(analysers[i]->chkFrameRate() == frameRate
			 && analysers[i]->chkPrefs().getTemporalWindow() == prefs.getTemporalWindow()
			 && analysers[i]->chkPrefs().getBpo() == prefs.getBpo()
			 && analysers[i]->chkPrefs().getBinFreq(0) == prefs.getBinFreq(0)
			 && analysers[i]->chkPrefs().getOctaves() == prefs.getOctaves()
			 && analysers[i]->chkPrefs().getOctaveOffset() == prefs.getOctaveOffset()
		){
			if(analysers[i]->chkPrefs().getSpectrumAnalyser() == 'f'
				 && prefs.getSpectrumAnalyser() == 'f'
				 && analysers[i]->chkPrefs().getFftFrameSize() == prefs.getFftFrameSize()){
				// FFTW
				if(analysers[i]->chkPrefs().getFftPostProcessor() == 'i'
					 && prefs.getFftPostProcessor() == 'i'
					 && analysers[i]->chkPrefs().getDirectSkStretch() == prefs.getDirectSkStretch()){
					// DirectSK
					foundMatch = true;
				}else if(analysers[i]->chkPrefs().getFftPostProcessor() == prefs.getFftPostProcessor()){
					// CQT
					foundMatch = true;
				}
			}else if(analysers[i]->chkPrefs().getSpectrumAnalyser() == 'g'
				 && prefs.getSpectrumAnalyser() == 'g'
				 && analysers[i]->chkPrefs().getGoertzelMinK() == prefs.getGoertzelMinK()){
				// Goertzel
				foundMatch = true;
			}
		}
		if(foundMatch){
			return analysers[i]->getSpectrumAnalyser();
		}
	}
	if(prefs.getSpectrumAnalyser() == 'g')
		analysers.push_back(new SpectrumAnalyserWrapper(frameRate, new GoertzelAnalyser(frameRate,prefs)));
	else
		analysers.push_back(new SpectrumAnalyserWrapper(frameRate, new FftwAnalyser(frameRate,prefs)));
	return analysers[analysers.size()-1]->getSpectrumAnalyser();
}
