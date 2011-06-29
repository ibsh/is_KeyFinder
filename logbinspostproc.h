#ifndef LOGBINSPOSTPROC_H
#define LOGBINSPOSTPROC_H

#include "fftpostprocessor.h"
#include "preferences.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>

class LogBinsPostProc : public FftPostProcessor{
public:
	LogBinsPostProc(int, const Preferences&);
	virtual std::vector<float> chromaVector(fftw_complex*)const;
	virtual void printKernel()const;
private:
	std::vector<int> reqbins;
};

#endif
