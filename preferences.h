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
	void setHopSize(int);
	void setFftFrameSize(int);
	void setBandsPerSemitone(int);
	void setDownsampleFactor(int);
	void setDirectSkStretch(float);
	// getters
	char getSpectrumAnalyser() const;
	char getFftPostProcessor() const;
	char getTemporalWindow() const;
	int getHopSize() const;
	int getFftFrameSize() const;
	int getGoertzelMinK() const;
	int getOctaves() const;
	int getBpo() const;
	int getDFactor() const;
	int getToneProfile() const;
	int getHcdfPeakPickingNeighbours() const;
	int getHcdfGaussianSize() const;
	float getHcdfGaussianSigma() const;
	float getBinFreq(int) const;
	float getDirectSkStretch() const;
	float getDetunedBandWeight() const;
private:
	char spectrumAnalyser;
	char fftPostProcessor;
	char temporalWindow;
	int hopSize;
	int fftFrameSize;
	int goertzelMinK;
	int octaves;
	int bps;
	int dFactor;
	int toneProfile;
	int hcdfPeakPickingNeighbours;
	int hcdfGaussianSize;
	float hcdfGaussianSigma;
	float stFreq;
	float directSkStretch;
	float detunedBandWeight;
	std::vector<float> binFreqs;
	void generateBinFreqs();
};

#endif
