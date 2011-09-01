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

#include "hcdfcosine.h"

CosineHcdf::CosineHcdf(){
	pi = (4 * atan(1.0));
}

std::vector<double> CosineHcdf::getRateOfChange(Chromagram* ch, const Preferences& prefs){
	int hops = ch->getHops();
	int bins = ch->getBins();
	int gaussianSize = prefs.getHcdfGaussianSize();
	double gaussianSigma = prefs.getHcdfGaussianSigma();
	int padding = 0; // as opposed to gaussianSize/2
	std::vector<double> cosine(hops+padding);
	for(int hop=0; hop<hops; hop++){
		double top = 0.0;
		double bottom = 0.0;
		for(int bin=0; bin<bins; bin++){
			float mag = ch->getMagnitude(hop,bin);
			top += mag;
			bottom += pow(mag,2);
		}
		double cos;
		if(bottom > 0.0) // divzero
			cos = top / sqrt(bottom) * sqrt(bins*sqrt(2));
		else
			cos = 0.0;
		cosine[hop] = cos;
	}
	// gaussian
	std::vector<double> gaussian(gaussianSize);
	for(int i=0; i<gaussianSize; i++){
		gaussian[i] = exp(-1 * (pow(i-gaussianSize/2,2) / (2 * gaussianSigma * gaussianSigma)));
	}
	std::vector<double> smoothed(hops);
	for(int hop=padding; hop<(signed)cosine.size(); hop++){
		double conv = 0.0;
		for(int k=0; k<gaussianSize; k++){
			int frm = hop - (gaussianSize/2) + k;
			if(frm >= 0 && frm < (signed)cosine.size()){
				conv += cosine[frm] * gaussian[k];
			}
		}
		smoothed[hop-padding] = conv;
	}
	// RATE OF CHANGE OF HCDF SIGNAL; look at all hops except first.
	std::vector<double> rateOfChange(hops);
	for(int hop=1; hop<hops; hop++){
		double change = (smoothed[hop] - smoothed[hop-1]) / 90.0;
		change = (change >= 0 ? change : change * -1.0);
		change = change/0.16; // magic number; for display purposes only
		rateOfChange[hop] = change;
	}
	// fudge first
	rateOfChange[0] = rateOfChange[1];
	return rateOfChange;
}

std::vector<int> CosineHcdf::getSegments(const std::vector<double>& rateOfChange, const Preferences& prefs){
	// Pick peaks
	std::vector<int> changes(1); // start vector with a 0 to enable first classification
	int neighbours = prefs.getHcdfPeakPickingNeighbours();
	for(int hop=0; hop<(signed)rateOfChange.size(); hop++){
		bool peak = true;
		for(int i=-neighbours; i<=neighbours; i++)
			if(i!=0 && hop+i >=0 && hop+i < (signed)rateOfChange.size()) // only test valid neighbours
				if(rateOfChange[hop+i] >= rateOfChange[hop])
					peak = false; // there's a neighbour of higher or equal value; this isn't a peak
		if(peak)
			changes.push_back(hop);
	}
	return changes;
}

