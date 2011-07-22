#include "hcdf.h"

Hcdf::Hcdf(){
	pi = (4 * atan(1.0));
}

double Hcdf::phi(int d, int l){
	int circle = d/2;
	double result = l * pi;
	double radius = 0.0;
	if(circle == 0){
		result *= (7.0/6.0);
		radius = 1.0;
	}else if(circle == 1){
		result *= (3.0/2.0);
		radius = 1.0;
	}else if(circle == 2){
		result *= (2.0/3.0);
		radius = 0.5;
	}
	if(d%2 == 0){
		return (radius * sin(result));
	}else{
		return (radius * cos(result));
	}
}

std::vector<double> Hcdf::hcdf(Chromagram* ch, const Preferences& prefs){
	int bpo = 12; // note this function must always be applied *after* decomposition to 1 bps.
	int hops = ch->getHops();
	int dims = 6;
	int gaussianSize = prefs.getHcdfGaussianSize();
	double gaussianSigma = prefs.getHcdfGaussianSigma();
	// build vector for movement in six dimensions
	// zero pad end of vector by half width of smoothing Gaussian? Haven't done that.
	int padding = 0; // as opposed to gaussianSize/2
	std::vector<std::vector<double> > zeta(hops+padding,std::vector<double>(dims));
	for(int hop=0; hop<hops; hop++){
		double taxicabNorm = 0.0;
		for(int bin=0; bin<bpo; bin++){
			taxicabNorm += abs((double)ch->getMagnitude(hop,bin));
		}
		for(int d=0; d<dims; d++){
			double sum = 0.0;
			for(int bin=0; bin<bpo; bin++){
				sum += (phi(d,bin) * (double)ch->getMagnitude(hop,bin));
			}
			double result = sum / taxicabNorm;
			if(result != result || result < -1.0 || result > 1.0)
				result = 0.0; // takes care of NaN and/or +/- infinity where track is close to silent.
			zeta[hop][d] = result;
		}
	}
	// gaussian
	std::vector<double> gaussian(gaussianSize);
	for(int i=0; i<gaussianSize; i++){
		// maybe. This is the def from http://en.wikipedia.org/wiki/Gaussian_function
		gaussian[i] = exp(-1 * (pow(i-gaussianSize/2,2) / (2 * gaussianSigma * gaussianSigma)));
		// there's also http://en.wikipedia.org/wiki/Window_function#Gaussian_windows
		// and http://www.mathworks.com/help/toolbox/signal/gausswin.html
	}
	// convolution with gaussian according to http://cnx.org/content/m11539/latest/
	std::vector<std::vector<double> > smoothed(hops,std::vector<double>(dims));
	for(int d=0; d<dims; d++){
		for(int hop=padding; hop<(signed)zeta.size(); hop++){
			double conv = 0.0;
			for(int k=0; k<gaussianSize; k++){
				int frm = hop - (gaussianSize/2) + k;
				if(frm >= 0 && frm < (signed)zeta.size()){
					conv += zeta[frm][d] * gaussian[k];
				}
			}
			smoothed[hop-padding][d] = conv;
		}
	}
	// RATE OF CHANGE OF HCDF SIGNAL; look at all hops except first and last.
	std::vector<double> rateOfChange(hops);
	for(int hop=1; hop<hops; hop++){ // was hop<hops-1
		double xi = 0.0;
		for(int d=0; d<dims; d++){
			xi += pow((smoothed[hop][d] - smoothed[hop-1][d]),2); // was hop+1 vs hop-1
		}
		xi = sqrt(xi);
		rateOfChange[hop] = xi;
	}
	// fudge first and last
	rateOfChange[0] = rateOfChange[1];
	rateOfChange[hops-1] = rateOfChange[hops-2];
	return rateOfChange;
}

std::vector<int> Hcdf::peaks(const std::vector<double>& rateOfChange, const Preferences& prefs){
	// Pick peaks
	std::vector<int> changes(1); // start vector with a 0 to enable first classification
	int neighbours = prefs.getHcdfPeakPickingNeighbours();
	for(int hop=0; hop<(signed)rateOfChange.size(); hop++){
		bool peak = true;
		for(int i=-neighbours; i<=neighbours; i++)
			if(i!=0 && hop+i >=0 && hop+i < (signed)rateOfChange.size()) // only test valid neighbours
				if(rateOfChange[hop+i] >= rateOfChange[hop])
					peak = false; // there's a neighbour of higher value; this isn't a peak
		if(peak)
			changes.push_back(hop);
	}
	return changes;
}
