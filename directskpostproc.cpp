#include "directskpostproc.h"

using std::cout;
using std::cerr;
using std::endl;
using std::fixed;
using std::setprecision;

DirectSkPostProc::DirectSkPostProc(int frameRate, const Preferences& prefs) : FftPostProcessor(frameRate, prefs) {
	pi = (4 * atan(1.0));
	binOffsets = std::vector<int>(bins);
	norms = std::vector<float>(bins);
	mySpecKernel = std::vector<std::vector<float> >(bins,std::vector<float>(0));
	float qStretch = prefs.getDirectSkStretch();
	float qFactor = (1.0/qStretch) * 1.0/(pow(2,(1.0 / prefs.getBpo()))-1);
	for(int i=0; i<bins; i++){
		float centreOfWindow = prefs.getBinFreq(i) * fftFrameSize / frameRate;
		float widthOfWindow = prefs.getBinFreq(i) / qFactor * fftFrameSize / frameRate;
		float beginningOfWindow = centreOfWindow - (widthOfWindow / 2);
		float endOfWindow = centreOfWindow + (widthOfWindow / 2);
		for(int thisFftBin = 0; thisFftBin < fftFrameSize; thisFftBin++){
			if((float)thisFftBin >= beginningOfWindow && (float)thisFftBin <= endOfWindow){
				if(binOffsets[i] == 0)
					binOffsets[i] = thisFftBin;
				float coefficient = specialWindow((float)thisFftBin-beginningOfWindow,widthOfWindow);
				norms[i] += coefficient / prefs.getBinFreq(i); // interesting. This closely mirrors CQT, only cleaner.
				mySpecKernel[i].push_back(coefficient);
			}
		}
	}
}

std::vector<float> DirectSkPostProc::chromaVector(fftw_complex* fftResult)const{
	std::vector<float> cv(bins);
	for(int i=0; i<bins; i++){
		float sum = 0.0;
		for(int j=0; j<mySpecKernel[i].size(); j++){
			int binNum = binOffsets[i]+j;
			float real = fftResult[binNum][0];
			float imag = fftResult[binNum][1];
			float magnitude = sqrt((real*real)+(imag*imag));
			sum += (magnitude * mySpecKernel[i][j]) / norms[i];
		}
		cv[i] = sum;
	}
	return cv;
}

float DirectSkPostProc::specialWindow(float n, float N)const{
	// special because it deals in floats and is one element longer than int window functions
	return 0.5 * (1.0 - cos((2 * pi * n)/N)); // Hann window
}

void DirectSkPostProc::printKernel()const{
	cout << fixed;
 	int verylastFftBin = binOffsets[bins-1] + mySpecKernel[bins-1].size() - 1;
	for(int i=0; i<bins; i++){
		for(int j=0; j<=verylastFftBin; j++){
			if(j < binOffsets[i]){
				cout << "0 ";
			}else if(j < binOffsets[i] + mySpecKernel[i].size()){
				//float out = mySpecKernel[i][(j-binOffsets[i])] / norms[i];
				float out = mySpecKernel[i][(j-binOffsets[i])];
				cout << setprecision(3) << out << " ";
			}else{
				cout << "0 ";
			}
		}
		cout << endl;
	}
}
