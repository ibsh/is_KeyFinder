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
	bps = 3;													// DEFAULT 3. Not always the best idea but it's as accurate as 1 now.
	dFactor = 10;
	toneProfile = 2;									// DEFAULT 2
	stFreq = 27.5;
	directSkStretch = 1.0;
	hcdfGaussianSize = 35;						// DEFAULT 35. Originally 19 but this gets fewer misses.
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
int Preferences::getHopSize()const{return hopSize;}
int Preferences::getFftFrameSize()const{return fftFrameSize;}
int Preferences::getGoertzelMinK()const{return goertzelMinK;}
int Preferences::getOctaves()const{return octaves;}
int Preferences::getBpo()const{return bps * 12;}
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
		std::cerr << "Requested freq " << n << " out of bounds" << std::endl;
		return 0;
	}
	return binFreqs[n];
}

float Preferences::getLastFreq() const{
	return binFreqs[octaves*12*bps-1];
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
		// tune down for bins before first concert pitch bin (if bps > 1)
		for(int j=0; j<concertPitchBin; j++){
			binFreqs[(i*bpo)+j] = binFreq / pow(freqRatio,(concertPitchBin-j));
		}
		for(int j=concertPitchBin; j<bpo; j++){
			binFreqs[(i*bpo)+j] = binFreq;
			binFreq *= freqRatio;
		}
		octFreq *= 2;
	}
}


/*
void Preferences::printOptions(char* argv[])const{
	cerr << endl << "Usage: " << argv[0] << " [options] infile" << endl;
	cerr << "where options are:" << endl;
	cerr << "Force a particular decoder:" << endl;
	cerr << " -c s	: libsndfile (Default for WAV, AIFF, FLAC)" << endl;
	cerr << " -c a	: libavcodec (Default for all other file types)" << endl;
	cerr << "Force a particular downsampler:" << endl;
	cerr << " -d i	: Decimator with hardcoded coefficients (Default for 44100Hz, df 10)" << endl;
	cerr << " -d s	: libsamplerate (Default for all other samplerates and factors)" << endl;
	cerr << " -d d	: Sample discarder (aliases badly)" << endl;
	cerr << "Choice of temporal window function:" << endl;
	cerr << " -w m	: Hamming (Default)" << endl;
	cerr << " -w n	: Hann" << endl;
	cerr << " -w b	: Blackman" << endl;
	cerr << "Choice of transform algorithm:" << endl;
	cerr << " -t f	: FFTW (Default)" << endl;
	cerr << " -t g	: Goertzel" << endl;
	cerr << "Choice of FFT post-processor:" << endl;
	cerr << " -p i	: Direct kernel (Default)" << endl;
	cerr << " -p c	: Constant Q" << endl;
	cerr << " -p l	: Individual logarithmically-spaced bins" << endl;
	cerr << "Configuration options (defaults in parens):" << endl;
	cerr << " -h N	: Hop size (16384)" << endl;
	cerr << " -f N	: FFT frame size (32768)" << endl;
	cerr << " -k N	: Goertzel minimum k (20)" << endl;
	cerr << " -s N	: Starting frequency in Hz (27.5)" << endl;
	cerr << " -o N	: Octaves (6)" << endl;
	cerr << " -b N	: Bands per semitone (1)" << endl;
	cerr << " -df N	: Downsample factor (10)" << endl;
	cerr << " -ks N	: Direct kernel QStretch (1.0)" << endl;
	cerr << " -kw X	: Direct kernel window (Hann)" << endl;
}
*/
