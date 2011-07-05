#include "hcdfharte.h"

HarteHCDF::HarteHCDF(){
	pi = (4 * atan(1.0));
}

double HarteHCDF::phi(int d, int l){
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

std::vector<int> HarteHCDF::hcdf(Chromagram* ch){
	// TODO add validation or generalise; only works for ch = 12 bins.
	Preferences* prefs = new Preferences(); // FIX
	int bpo = prefs->getBpo();
	int hops = ch->getHops();
	int dims = 6;
	// build vector for movement in six dimensions
	std::vector<std::vector<double> > zeta(hops,std::vector<double>(dims));
	for(int hop=0; hop<hops; hop++){
		double taxicabNorm = 0;
		for(int bin=0; bin<bpo; bin++){
			taxicabNorm += abs(ch->getMagnitude(hop,bin));
		}
		for(int d=0; d<dims; d++){
			double sum = 0;
			for(int bin=0; bin<bpo; bin++){
				sum += (phi(d,bin) * ch->getMagnitude(hop,bin));
			}
			zeta[hop][d] = sum / taxicabNorm;
		}
	}
	// gaussian
	int gaussianSize = 19; // 19 standard
	double gaussianSigma = 8.0;
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
		for(int hop=0; hop<hops; hop++){
			double conv = 0.0;
			for(int k=0; k<gaussianSize; k++){
				int frm = hop - gaussianSize/2 + k;
				if(frm >= 0 && frm < hops){
					conv += zeta[frm][d] * gaussian[k];
				}
			}
			smoothed[hop][d] = conv;
		}
	}
	// RATE OF CHANGE OF HCDF SIGNAL; looking at all hops except first and last, for the moment.
	std::vector<double> rateOfChange(hops);
	for(int hop=1; hop<hops-1; hop++){
		double xi = 0.0;
		for(int d=0; d<dims; d++){
			xi += pow((smoothed[hop+1][d] - smoothed[hop-1][d]),2);
		}
		xi = sqrt(xi);
		if(xi==xi) // NaN check, underflow related I think?
			rateOfChange[hop] = xi;
		else
			rateOfChange[hop] = 0;
	}
	// Pick peaks
	std::vector<int> changes(1); // start vector with a 0 so first section can be classified simply
	int neighbours = 4;
	for(int hop=neighbours; hop<hops-neighbours; hop++){
		bool peak = true;
		for(int i=-neighbours; i<=neighbours; i++)
			if(i != 0 && rateOfChange[hop+i] >= rateOfChange[hop])
				peak = false; // there's a neighbour of higher value; this isn't a peak
		if(peak)
			changes.push_back(hop);
	}
	return changes;
}
