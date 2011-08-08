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
