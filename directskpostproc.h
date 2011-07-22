#ifndef DIRECTSKPOSTPROC_H
#define DIRECTSKPOSTPROC_H

#include "fftpostprocessor.h"
#include "preferences.h"
#include "windowfunctions.h"
#include <math.h>
#include <iomanip>
#include <stdlib.h>
#include <vector>


class DirectSkPostProc : public FftPostProcessor{
public:
	DirectSkPostProc(int, const Preferences&);
	virtual std::vector<float> chromaVector(fftw_complex*)const;
	virtual void printKernel()const;
private:
	std::vector<std::vector<float> > mySpecKernel; // ragged 2D array; narrow for bass, wide for treble.
	std::vector<int> binOffsets; // which fft bin to multiply by first coefficient.
	float kernelWindow(float,float)const;
	float pi;
};

#endif
