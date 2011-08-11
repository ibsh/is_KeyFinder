/*************************************************************************

	Copyright 2011 Ibrahim Sha'ath

	This file is part of KeyFinder.

	KeyFinder is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	KeyFinder is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with KeyFinder.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#include "constantqpostproc.h"

ConstantQPostProc::ConstantQPostProc(int frameRate, const Preferences& prefs) : FftPostProcessor(frameRate, prefs) {
	pi = (4 * atan(1.0));
	binOffsets = std::vector<int>(bins);
	sparseKernel = std::vector<std::vector<std::complex<float> > > (bins);
	float sparseThresh = 0.0054;
	float qFactor = 1.0 / (pow(2,(1.0 / prefs.getBpo()))-1);
	WindowFunction* w = WindowFunction::getWindowFunction('m'); // Hamming
	fftw_complex* tempKernel = (fftw_complex*)fftw_malloc(fftFrameSize*sizeof(fftw_complex));
	fftw_complex* specKernel = (fftw_complex*)fftw_malloc(fftFrameSize*sizeof(fftw_complex));
	fftw_plan planCq = fftw_plan_dft_1d(fftFrameSize, tempKernel, specKernel, FFTW_FORWARD, FFTW_ESTIMATE);
	// build
	for(int i=bins-1; i>=0; i--){ // Why backwards? I don't know. But it screws up the kernel going forwards.
		float freqI = prefs.getBinFreq(i);
		int lengthI = ceil(qFactor * frameRate / freqI);
		// Harte differs from Blankertz; calculates offsets so that temporal kernels are centred in their windows
		int offsetI;
		if(lengthI % 2 == 0)
			offsetI = (fftFrameSize/2)-(lengthI/2);
		else
			offsetI = (fftFrameSize/2)-((lengthI+1)/2);
		for(int j=0; j<fftFrameSize; j++){
			tempKernel[j][0] = 0.0;
			tempKernel[j][1] = 0.0;
		}
		if(offsetI<0){
			qFatal("Fatal parameter mismatch in CQT construction. FFT frame size %d below implicit minimum %d.", fftFrameSize, lengthI+1);
		}
		for(int j=0; j<lengthI; j++){
			float winj = w->window(j,lengthI) / lengthI;
			tempKernel[offsetI+j][0] = winj * cos((2*pi*j*qFactor)/lengthI); // real part
			tempKernel[offsetI+j][1] = winj * sin((2*pi*j*qFactor)/lengthI); // imag part
		}
		// FFT : should this be made a single 2D run rather than 72 1D runs?
		fftw_execute(planCq);
		std::vector<std::complex<float> > kernelRow;
		for(int j=0; j<fftFrameSize; j++){
			float real = specKernel[j][0];
			float imag = specKernel[j][1];
			float magnitude = sqrt((real*real)+(imag*imag));
			if(magnitude > sparseThresh){
				if(binOffsets[i] == 0) binOffsets[i] = j;
				// This is in the spec but it leads to arithmetic underflow without any advantage.
				// real /= fftFrameSize;
				// imag /= fftFrameSize;
				imag *= -1.0; // complex conjugate
				std::complex<float> kernelEntry(real,imag);
				kernelRow.push_back(kernelEntry);
			}
		}
		sparseKernel[i] = kernelRow;
	}
	delete w;
	fftw_destroy_plan(planCq);
	fftw_free(tempKernel);
	fftw_free(specKernel);
}

std::vector<float> ConstantQPostProc::chromaVector(fftw_complex* fftResult)const{
	std::vector<float> cv(bins);
	for(int i=0; i<bins; i++){
		float sum = 0.0;
		for(int j=0; j<(signed)sparseKernel[i].size(); j++){
			int binNum = binOffsets[i]+j;
			float real = fftResult[binNum][0] * sparseKernel[i][j].real();
			float imag = fftResult[binNum][1] * sparseKernel[i][j].imag();
			float magnitude = sqrt((real*real)+(imag*imag));
			sum += magnitude;
		}
		cv[i] = sum;
	}
	return cv;
}

void ConstantQPostProc::printKernel()const{
	std::cout << std::fixed;
	int verylastFftBin = binOffsets[bins-1] + sparseKernel[bins-1].size() - 1;
	for(int i=0; i<bins; i++){
		for(int j=0; j<=verylastFftBin; j++){
			if(j < binOffsets[i]){
				std::cout << "0 ";
			}else if(j < binOffsets[i] + (signed)sparseKernel[i].size()){
				float real = sparseKernel[i][j-binOffsets[i]].real();
				float imag = sparseKernel[i][j-binOffsets[i]].imag();
				float magnitude = sqrt((real*real)+(imag*imag));
				std::cout << std::setprecision(3) << magnitude << " ";
			}else{
				std::cout << "0 ";
			}
		}
		std::cout << std::endl;
	}
}
