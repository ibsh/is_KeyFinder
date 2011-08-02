#include "directskpostproc.h"

DirectSkPostProc::DirectSkPostProc(int frameRate, const Preferences& prefs) : FftPostProcessor(frameRate, prefs) {
	pi = (4 * atan(1.0));
	binOffsets = std::vector<int>(bins);
	mySpecKernel = std::vector<std::vector<float> >(bins,std::vector<float>(0));
	std::vector<float> sums = std::vector<float>(bins);
	float qFactor = 1.0 / (prefs.getDirectSkStretch() * (pow(2,(1.0 / prefs.getBpo()))-1));
	for(int i=0; i<bins; i++){
		float centreOfWindow = prefs.getBinFreq(i) * fftFrameSize / frameRate;
		float widthOfWindow = centreOfWindow / qFactor;
		float beginningOfWindow = centreOfWindow - (widthOfWindow / 2);
		float endOfWindow = beginningOfWindow + widthOfWindow;
		for(int thisFftBin = 0; thisFftBin < fftFrameSize; thisFftBin++){
			if((float)thisFftBin < beginningOfWindow)
				continue; // haven't got to useful fft bins yet
			if((float)thisFftBin > endOfWindow)
				break; // finished with useful fft bins
			if(binOffsets[i] == 0)
				binOffsets[i] = thisFftBin; // first useful fft bin
			float coefficient = kernelWindow(((float)thisFftBin)-beginningOfWindow,widthOfWindow);
			sums[i] += coefficient;
			mySpecKernel[i].push_back(coefficient);
		}
		// normalisation by sum of coefficients and frequency of bin; models CQT very closely
		for(int j=0; j<(signed)mySpecKernel[i].size(); j++)
			mySpecKernel[i][j] = (mySpecKernel[i][j] / sums[i]) * prefs.getBinFreq(i);
	}
}

std::vector<float> DirectSkPostProc::chromaVector(fftw_complex* fftResult)const{
	std::vector<float> cv(bins);
	for(int i=0; i<bins; i++){
		float sum = 0.0;
		for(int j=0; j<(signed)mySpecKernel[i].size(); j++){
			int binNum = binOffsets[i]+j;
			float real = fftResult[binNum][0];
			float imag = fftResult[binNum][1];
			float magnitude = sqrt((real*real)+(imag*imag));
			sum += (magnitude * mySpecKernel[i][j]);
		}
		cv[i] = sum;
	}
	return cv;
}

float DirectSkPostProc::kernelWindow(float n, float N)const{
	// discretely sampled continuous function, but different to other window functions
	return 1.0 - cos((2 * pi * n)/N); // based on Hann; no need to halve since we normalise later
}

void DirectSkPostProc::printKernel()const{
//	std::cout << std::fixed;
//	std::cout << "mysk offset " << binOffsets[0] << " length " << mySpecKernel[0].size() << std::endl;
//	for(int j=0; j<(signed)mySpecKernel[0].size(); j++){
//		float out = mySpecKernel[0][j];
//		std::cout << std::setprecision(3) << out << "\t";
//	}
//	std::cout << "offset " << binOffsets[11] << " length " << mySpecKernel[11].size() << std::endl;
//	std::cout << std::endl;
//	for(int j=0; j<(signed)mySpecKernel[11].size(); j++){
//		float out = mySpecKernel[11][j];
//		std::cout << std::setprecision(3) << out << "\t";
//	}
//	std::cout << std::endl;
//	return;
	// original fn
	std::cout << std::fixed;
	int verylastFftBin = binOffsets[bins-1] + mySpecKernel[bins-1].size() - 1;
	for(int i=0; i<bins; i++){
		for(int j=0; j<=verylastFftBin; j++){
			if(j < binOffsets[i]){
				std::cout << "0 ";
			}else if(j < binOffsets[i] + (signed)mySpecKernel[i].size()){
				float out = mySpecKernel[i][j-binOffsets[i]];
				std::cout << std::setprecision(3) << out << " ";
			}else{
				std::cout << "0 ";
			}
		}
		std::cout << std::endl;
	}
}
