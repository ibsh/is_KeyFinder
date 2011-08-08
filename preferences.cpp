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

#include "preferences.h"

Preferences::Preferences(){
	// DEFAULT VALUES
	temporalWindow = 'b';							// DEFAULT b. Blackman
	spectrumAnalyser = 'f';						// DEFAULT f. FFTW
	fftPostProcessor = 'i';						// DEFAULT i. My quick spectral kernel
	fftFrameSize = 65536;							// DEFAULT 65536. This is large, obviously. But reducing it reduces accuracy.
	hopSize = fftFrameSize / 4;				// DEFAULT 1/4 of framesize.
	goertzelMinK = 60;
	octaves = 6;
	bps = 1;													// DEFAULT 1. 3 only decreases accuracy, all other things being equal.
	octaveOffset = 0;									// DEFAULT 0. 3 gets an octave that starts at C, but accuracy drops a little.
	dFactor = 10;											// DEFAULT 10
	toneProfile = 2;									// DEFAULT 2
	stFreq = 27.5;
	directSkStretch = 1.0;						// DEFAULT 1.0, 3.8 to closely model CQT.
	hcdf = 'n';												// DEFAULT h. Harte
	hcdfGaussianSize = 35;						// DEFAULT 35
	hcdfGaussianSigma = 8.0;
	hcdfPeakPickingNeighbours = 4;		// DEFAULT 4
	tuningMethod = 2;									// DEFAULT 2
	detunedBandWeight = 0.5;
	generateBinFreqs();
}

void Preferences::setSpectrumAnalyser(char c){
	if (c == 'f' || c == 'g')
		spectrumAnalyser = c;
}

void Preferences::setFftPostProcessor(char c){
	if (c == 'i' || c == 'c' || c == 'l')
		fftPostProcessor = c;
}

void Preferences::setTemporalWindow(char c){
	if (c == 'm' || c == 'n' || c == 'b')
		temporalWindow = c;
}

void Preferences::setToneProfile(int n){
	if(n >= 0 && n <= 2)
		toneProfile = n;
}

void Preferences::setHopSize(int n){
	hopSize = n;
}

void Preferences::setFftFrameSize(int n){
	fftFrameSize = n;
}

void Preferences::setBandsPerSemitone(int n){
	if(n%2!=0){
		bps = n;
		generateBinFreqs();
	}
}

void Preferences::setDownsampleFactor(int n){
	dFactor = n;
}

void Preferences::setDirectSkStretch(float n){
	directSkStretch = n;
}


char Preferences::getSpectrumAnalyser()const{return spectrumAnalyser;}
char Preferences::getFftPostProcessor()const{return fftPostProcessor;}
char Preferences::getTemporalWindow()const{return temporalWindow;}
char Preferences::getHcdf()const{return hcdf;}
int Preferences::getHopSize()const{return hopSize;}
int Preferences::getFftFrameSize()const{return fftFrameSize;}
int Preferences::getGoertzelMinK()const{return goertzelMinK;}
int Preferences::getOctaves()const{return octaves;}
int Preferences::getBpo()const{return bps * 12;}
int Preferences::getOctaveOffset()const{return octaveOffset;}
int Preferences::getDFactor()const{return dFactor;}
int Preferences::getToneProfile()const{return toneProfile;}
int Preferences::getTuningMethod()const{return tuningMethod;}
int Preferences::getHcdfPeakPickingNeighbours()const{return hcdfPeakPickingNeighbours;}
int Preferences::getHcdfGaussianSize()const{return hcdfGaussianSize;}
float Preferences::getHcdfGaussianSigma()const{return hcdfGaussianSigma;}
float Preferences::getDirectSkStretch()const{return directSkStretch;}
float Preferences::getDetunedBandWeight()const{return detunedBandWeight;}

float Preferences::getBinFreq(int n)const{
	if(n >= octaves*12*bps){
		qDebug("Attempt to get out-of-bounds frequency index (%d/%d)",n,octaves*12*bps);
		return 0;
	}
	return binFreqs[n];
}

float Preferences::getLastFreq() const{
	return binFreqs[binFreqs.size()-1];
}

void Preferences::generateBinFreqs(){
	int bpo = bps * 12;
	binFreqs = std::vector<float>(octaves*bpo);
	float freqRatio = pow(2,(1.0/bpo));
	float octFreq = stFreq;
	float binFreq;
	int concertPitchBin = bps/2;
	for(int i=0; i<octaves; i++){
		binFreq = octFreq;
		// offset by specified number of semitones
		for(int j=0; j<octaveOffset*bps; j++)
			binFreq *= freqRatio;
		// tune down for bins before first concert pitch bin (if bps > 1)
		for(int j=0; j<concertPitchBin; j++)
			binFreqs[(i*bpo)+j] = binFreq / pow(freqRatio,(concertPitchBin-j));
		// and tune all other bins
		for(int j=concertPitchBin; j<bpo; j++){
			binFreqs[(i*bpo)+j] = binFreq;
			binFreq *= freqRatio;
		}
		octFreq *= 2;
	}
}


/*
void Preferences::printOptions(char* argv[])const{
	std::cout << std::endl << "Usage: " << argv[0] << " [options] infile" << std::endl;
	std::cout << "where options are:" << std::endl;
	std::cout << "Force a particular decoder:" << std::endl;
	std::cout << " -c s	: libsndfile (Default for WAV, AIFF, FLAC)" << std::endl;
	std::cout << " -c a	: libavcodec (Default for all other file types)" << std::endl;
	std::cout << "Force a particular downsampler:" << std::endl;
	std::cout << " -d i	: Decimator with hardcoded coefficients (Default for 44100Hz, df 10)" << std::endl;
	std::cout << " -d s	: libsamplerate (Default for all other samplerates and factors)" << std::endl;
	std::cout << " -d d	: Sample discarder (aliases badly)" << std::endl;
	std::cout << "Choice of temporal window function:" << std::endl;
	std::cout << " -w m	: Hamming (Default)" << std::endl;
	std::cout << " -w n	: Hann" << std::endl;
	std::cout << " -w b	: Blackman" << std::endl;
	std::cout << "Choice of transform algorithm:" << std::endl;
	std::cout << " -t f	: FFTW (Default)" << std::endl;
	std::cout << " -t g	: Goertzel" << std::endl;
	std::cout << "Choice of FFT post-processor:" << std::endl;
	std::cout << " -p i	: Direct kernel (Default)" << std::endl;
	std::cout << " -p c	: Constant Q" << std::endl;
	std::cout << " -p l	: Individual logarithmically-spaced bins" << std::endl;
	std::cout << "Configuration options (defaults in parens):" << std::endl;
	std::cout << " -h N	: Hop size (16384)" << std::endl;
	std::cout << " -f N	: FFT frame size (32768)" << std::endl;
	std::cout << " -k N	: Goertzel minimum k (20)" << std::endl;
	std::cout << " -s N	: Starting frequency in Hz (27.5)" << std::endl;
	std::cout << " -o N	: Octaves (6)" << std::endl;
	std::cout << " -b N	: Bands per semitone (1)" << std::endl;
	std::cout << " -df N	: Downsample factor (10)" << std::endl;
	std::cout << " -ks N	: Direct kernel QStretch (1.0)" << std::endl;
	std::cout << " -kw X	: Direct kernel window (Hann)" << std::endl;
}
*/
