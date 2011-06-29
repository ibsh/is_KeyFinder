#ifndef GOERTZELANALYSER_H
#define GOERTZELANALYSER_H

#include "spectrumanalyser.h"
#include "preferences.h"
#include "windowfunctions.h"
#include <math.h>
#include <vector>
#include <stdlib.h>

class GoertzelAnalyser : public SpectrumAnalyser{
public:
	GoertzelAnalyser(int, const Preferences&);
	~GoertzelAnalyser();
	virtual Chromagram* chromagram(AudioBuffer*);
private:
	class Goertzel;
	std::vector<Goertzel*> goertzels;
};

class GoertzelAnalyser::Goertzel{
public:
	Goertzel(float,int,int,char);
	void processSample(float);
	float getRelativeMagnitude() const;
	bool samplesWanted() const;
	void reset();
	int getN() const;
private:
	float frequency;
	float coeff;
	float sine;
	float cosine;
	float Q1, Q2;
	int N;
	int k;
	int samplesProcessed;
	std::vector<float> window;
};

#endif
