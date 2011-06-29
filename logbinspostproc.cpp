#include "logbinspostproc.h"

using std::cout;
using std::cerr;
using std::endl;
using std::fixed;
using std::setprecision;

LogBinsPostProc::LogBinsPostProc(int f, const Preferences& prefs) : FftPostProcessor(f, prefs) {
	reqbins = std::vector<int>(bins);
	for(int i=0; i<bins; i++){
		reqbins[i] = (int)((prefs.getBinFreq(i) * fftFrameSize / frameRate) + 0.5);
	}
}

std::vector<float> LogBinsPostProc::chromaVector(fftw_complex* fftResult)const{
	std::vector<float> cv(bins);
	for(int i=0; i<bins; i++){
		cv[i] = sqrt(pow(fftResult[reqbins[i]][0],2)+pow(fftResult[reqbins[i]][1],2));
	}
	return cv;
}

void LogBinsPostProc::printKernel()const{
	cout << fixed;
 	int verylastFftBin = reqbins[bins-1];
	for(int i=0; i<bins; i++){
		for(int j=0; j<=verylastFftBin; j++){
			if(j == reqbins[i]){
				cout << "1 ";
			}else{
				cout << "0 ";
			}
		}
		cout << endl;
	}
}
