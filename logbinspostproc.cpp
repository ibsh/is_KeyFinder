#include "logbinspostproc.h"

LogBinsPostProc::LogBinsPostProc(int f, const Preferences& prefs) : FftPostProcessor(f, prefs) {
	reqbins = std::vector<int>(bins);
	norms = std::vector<float>(bins);
	for(int i=0; i<bins; i++){
		reqbins[i] = (int)((prefs.getBinFreq(i) * fftFrameSize / frameRate) + 0.5);
		norms[i] = prefs.getBinFreq(i);
	}
}

std::vector<float> LogBinsPostProc::chromaVector(fftw_complex* fftResult)const{
	std::vector<float> cv(bins);
	for(int i=0; i<bins; i++){
		cv[i] = sqrt(pow(fftResult[reqbins[i]][0],2)+pow(fftResult[reqbins[i]][1],2));
		cv[i] *= norms[i];
	}
	return cv;
}

void LogBinsPostProc::printKernel()const{
	std::cout << std::fixed;
 	int verylastFftBin = reqbins[bins-1];
	for(int i=0; i<bins; i++){
		for(int j=0; j<=verylastFftBin; j++){
			if(j == reqbins[i]){
				std::cout << "1 ";
			}else{
				std::cout << "0 ";
			}
		}
		std::cout << std::endl;
	}
}
