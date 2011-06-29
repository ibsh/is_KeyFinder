#ifndef FFTPOSTPROCESSOR_H
#define FFTPOSTPROCESSOR_H

#include "preferences.h"
#include <fftw3.h>

class FftPostProcessor{
public:
	FftPostProcessor(int, const Preferences&);
	virtual std::vector<float> chromaVector(fftw_complex*) const = 0;
	virtual void printKernel() const = 0;
protected:
	int bins;
	int fftFrameSize;
	int frameRate;
};

#endif
