#ifndef FFTWANALYSER_H
#define FFTWANALYSER_H

#include "spectrumanalyser.h"
#include "fftpostprocessor.h"
#include "logbinspostproc.h"
#include "constantqpostproc.h"
#include "directskpostproc.h"
#include "windowfunctions.h"
#include <fftw3.h>

class FftwAnalyser : public SpectrumAnalyser{
public:
	FftwAnalyser(int, const Preferences&);
	~FftwAnalyser();
	virtual Chromagram* chromagram(AudioBuffer*);
private:
	int fftFrameSize;
	FftPostProcessor* pp;
	fftw_complex* fftInput;
	fftw_complex* fftResult;
	fftw_plan fftPlan;
	std::vector<float> window;
};

#endif
