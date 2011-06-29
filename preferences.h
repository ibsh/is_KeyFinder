#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <cstring>

// No longer a singleton once GUI implemented; made sense to keep one per window

class Preferences{
public:
	Preferences();
	// setters
	void setSpectrumAnalyser(char);
	void setFftPostProcessor(char);
	void setTemporalWindow(char);
	void setToneProfile(int);
	// getters
	char getSpectrumAnalyser() const;
	char getFftPostProcessor() const;
	char getTemporalWindow() const;
	char getChromaOrientation() const;
	char getDirectSkWindow() const;
	int getHopSize() const;
	int getFftFrameSize() const;
	int getGoertzelMinK() const;
	int getOctaves() const;
	int getBpo() const;
	int getDFactor() const;
	int getToneProfile() const;
	float getBinFreq(int) const;
	float getDirectSkStretch() const;
private:
	char spectrumAnalyser;
	char fftPostProcessor;
	char temporalWindow;
	char directSkWindow;
	int hopSize;
	int fftFrameSize;
	int goertzelMinK;
	int octaves;
	int bps;
	int dFactor;
	int toneProfile;
	float stFreq;
	float directSkStretch;
	std::vector<float> binFreqs;
	void generateBinFreqs();
};

#endif
