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

#include "spectrumanalysergoertzel.h"

GoertzelAnalyser::GoertzelAnalyser(int f, const Preferences& prefs): SpectrumAnalyser(f,prefs) {
	goertzels = std::vector<Goertzel*>(bins);
	for(int i=0; i<bins; i++)
		goertzels[i] = new Goertzel(prefs.getBinFreq(i),frameRate,prefs.getGoertzelMinK(),prefs.getTemporalWindow());
}

GoertzelAnalyser::~GoertzelAnalyser(){
	for(int i=0; i<bins; i++){
		delete goertzels[i];
	}
}

Chromagram* GoertzelAnalyser::chromagram(AudioBuffer* ab){
	QMutexLocker locker(&mutex); // Mutex this function
	/*
	New approach. Run every goertzel as many times as possible (zero padding during last run),
	getting magnitudes relating to windows of different lengths for each bin.
	Then take a normalised sum of the results and partial results that fall within a hop's
	window and put that into the chromagram.
	*/
	std::vector<std::vector<float> > magnitudes(bins,std::vector<float>(0));
	std::vector<bool> lastWindowDone(bins,false);
	bool finished = false;
	for(int i=0; !finished; i++){
		float sample = (i < ab->getSampleCount() ? ab->getSample(i) : 0);
		finished = true;
		for(int j=0; j<bins; j++){
			if(!goertzels[j]->samplesWanted()){ // this window complete, get output and reset
				magnitudes[j].push_back(goertzels[j]->getRelativeMagnitude() * 10000000.0); // avoid arithmetic underflow in HCDF
				if(i >= ab->getSampleCount()) //this was the last window for this Goertzel.
					lastWindowDone[j] = true;
				goertzels[j]->reset();
			}
			if(!lastWindowDone[j]){
				finished = false;
				goertzels[j]->processSample(sample);
			}
		}
	}
	Chromagram* ch = new Chromagram((ab->getSampleCount()/hopSize) + 1,bins);
	for(int i=0; i < ab->getSampleCount(); i += hopSize){
		int hopFirst = i;
		int hopLast = i + hopSize - 1;
		for(int j=0; j<(signed)magnitudes.size(); j++){
			int n = goertzels[j]->getN();
			float hopMag = 0.0;
			for(int k=0; k<(signed)magnitudes[j].size(); k++){
				int myFirst = k*n;
				int myLast = (k+1)*n-1;
				if(myFirst > hopLast) // this entry entirely after this hop, so we're done
					break;
				else if(myLast < hopFirst) // this entry entirely before this hop; on to the next
					continue;
				else if(myFirst >= hopFirst && myLast <= hopLast) // this entry entirely within this hop
					hopMag += magnitudes[j][k];
				else if(myFirst < hopFirst) // back part of this entry within this hop
					hopMag += magnitudes[j][k]*((myLast-hopFirst+1)/(float)n);
				else if(myLast > hopLast) // front part of this entry within this hop
					hopMag += magnitudes[j][k]*((hopLast-myFirst+1)/(float)n);
			}
			// normalise by number of goertzel results in hop
			hopMag /= hopSize;
			hopMag *= n;
			ch->setMagnitude(i/hopSize,j,hopMag);
		}
	}
	return ch;
}

GoertzelAnalyser::Goertzel::Goertzel(float freq, int frameRate, int minK, char whichWindow){
	float pi = (4 * atan(1.0));
	k = minK;
	float threshold = 0.2;
	while(true){
		float dblN = k / (freq/frameRate);
		float diff = (dblN - (int)dblN);
		if(diff < threshold){
			N = (int)dblN;
			break;
		}else if((1.0-diff) < threshold){
			N = (int)dblN + 1;
			break;
		}
		k++;
	}
	float theta = (2.0 * pi * k)/(float)N;
	sine = sin(theta);
	cosine = cos(theta);
	coeff = 2.0 * cosine;
	frequency = freq;
	WindowFunction* w = WindowFunction::getWindowFunction(whichWindow);
	window = std::vector<float>(N);
	for(int i=0; i<N; i++){
		window[i] = w->window(i,N);
	}
	reset();
}

void GoertzelAnalyser::Goertzel::processSample(float sample){
	sample *= window[samplesProcessed]; // self-windowing
	float Q0 = coeff * Q1 - Q2 + sample;
	Q2 = Q1;
	Q1 = Q0;
	samplesProcessed++;
}

float GoertzelAnalyser::Goertzel::getRelativeMagnitude() const{
	return sqrt(Q1*Q1 + Q2*Q2 - Q1*Q2 * coeff) / (N * k);
}

bool GoertzelAnalyser::Goertzel::samplesWanted() const{
	return (N - samplesProcessed) > 0;
}

void GoertzelAnalyser::Goertzel::reset(){
	Q1 = Q2 = 0.0;
	samplesProcessed = 0;
}

int GoertzelAnalyser::Goertzel::getN() const{
	return N;
}
