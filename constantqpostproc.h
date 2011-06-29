#ifndef CONSTANTQPOSTPROC_H
#define CONSTANTQPOSTPROC_H

#include "fftpostprocessor.h"
#include "preferences.h"
#include "windowfunctions.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <fftw3.h>
#include <vector>
#include <complex>

class ConstantQPostProc : public FftPostProcessor{
public:
	ConstantQPostProc(int, const Preferences&);
	virtual std::vector<float> chromaVector(fftw_complex*)const;
	virtual void printKernel()const;
private:
	std::vector<std::vector<std::complex<float> > > sparseKernel;
	std::vector<int> binOffsets;
	float pi;
};

#endif
