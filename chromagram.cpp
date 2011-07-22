#include "chromagram.h"

Chromagram::Chromagram(int h, int b){
	hops = h;
	bins = b;
	chroma = std::vector<float>(hops*bins);
}

void Chromagram::decomposeToTwelveBpo(const Preferences& prefs){
	int oct = prefs.getOctaves();
	int bps = (bins/oct)/12;
	if(bins == 12 * oct)
		return;
	int method = prefs.getTuningMethod();
	if(method==0){
		// this method is pretty involved, and uses some approximation of Harte's method.
		// find peaks; anything that's higher energy than the mean for this hop and higher energy than its neighbours.
		std::vector<std::vector<float> > peakLocations;
		std::vector<std::vector<float> > peakMagnitudes;
		for(int hop = 0; hop < hops; hop++){
			// find mean magnitude for this hop
			float meanVal = 0;
			for(int bin = 0; bin < bins; bin++){
				meanVal += getMagnitude(hop,bin);
			}
			meanVal /= bins;
			// find peak bins
			std::vector<int> peakBins;
			for(int bin = 1; bin < bins-1; bin++){
				float binVal = getMagnitude(hop,bin);
				// currently every peak over mean. Tried all peaks but accuracy dropped.
				if(binVal > meanVal && binVal > getMagnitude(hop,bin-1) && binVal > getMagnitude(hop,bin+1)){
					peakBins.push_back(bin);
				}
			}
			// quadratic interpolation to find a more precise peak position and magnitude.
			std::vector<float> peakLocationsRow;
			std::vector<float> peakMagnitudesRow;
			for(int peak=0; peak<(signed)peakBins.size(); peak++){
				float alpha = getMagnitude(hop,peakBins[peak] - 1);
				float beta  = getMagnitude(hop,peakBins[peak]);
				float gamma = getMagnitude(hop,peakBins[peak] + 1);
				float peakLocation = ((alpha - gamma)/(alpha - (2 * beta) + gamma))/2;
				float peakMagnitude = beta - ((1/4) * (alpha - gamma) * peakLocation);
				peakLocationsRow.push_back(peakBins[peak]+peakLocation);
				peakMagnitudesRow.push_back(peakMagnitude);
			}
			peakLocations.push_back(peakLocationsRow);
			peakMagnitudes.push_back(peakMagnitudesRow);
		}
		// determine tuning distribution of peaks. Centre bin = concert tuning.
		std::vector<float> peakTuningDistribution(bps*10);
		for(int hop = 0; hop < hops; hop++){
			for(int peak = 0; peak < (signed)peakLocations[hop].size(); peak++){
				float peakLocationMod = fmodf(peakLocations[hop][peak],(float)bps);
				peakLocationMod *= 10;
				int peakLocationInt = peakLocationMod + 0.5;
				peakLocationInt += 5;
				peakTuningDistribution[peakLocationInt%(bps*10)] += (peakMagnitudes[hop][peak] / 1000.0);
			}
		}
		// now find the tuning peak; the subdivision of a semitone that most peaks are tuned to.
		float tuningMax = 0;
		int tuningPeak = -1;
		for(int i=0; i<bps*10; i++){
			if(peakTuningDistribution[i] > tuningMax){
				tuningMax = peakTuningDistribution[i];
				tuningPeak = i;
			}
		}
		// now discard (zero out, for ease) any peaks that sit >= 0.2 semitones (6 bins for 3bps) away from the tuning peak.
		// build a chromagram to hold the new data
		std::vector<float> twelveBpoChroma(hops*12*oct);
		for(int i=0; i<hops*12*oct; i++)
			twelveBpoChroma[i] = 0.0;
		// figure out which tuning bins to keep
		std::vector<int> binsToKeep;
		for(int i=(1-(bps*2)); i<bps*2; i++)
			binsToKeep.push_back((tuningPeak + i + (bps*10)) % (bps*10));
		// and discard the others
		for(int hop = 0; hop < hops; hop++){
			for(int peak = 0; peak < (signed)peakLocations[hop].size(); peak++){
				float peakLocationMod = fmodf(peakLocations[hop][peak],(float)bps);
				peakLocationMod *= 10;
				int peakLocationInt = peakLocationMod + 0.5;
				peakLocationInt += 5;
				bool discardMe = true;
				for(int i=0; i<(signed)binsToKeep.size(); i++)
					if(peakLocationInt == binsToKeep[i])
						discardMe = false;
				if(!discardMe){ // this is a valid peak for the tuned chromagram
					int tunedPeakLocation = (int)peakLocations[hop][peak];
					tunedPeakLocation /= bps;
					twelveBpoChroma[(hop*12*oct) + tunedPeakLocation] += peakMagnitudes[hop][peak];
				}
			}
		}
		chroma = twelveBpoChroma;
		bins = 12 * oct;
		return;
	}else if(method==1){
		// this version is much more lightweight, and seems to perform better (x22 vs 23).
		std::vector<float> oneSemitoneChroma(bps);
		for(int h = 0; h < hops; h++)
			for(int st = 0; st < 12*oct; st++)
				for(int b = 0; b < bps; b++)
					oneSemitoneChroma[b] += getMagnitude(h,(st*bps)+b);
		// determine loudest tuning bin
		int whichBin = 0;
		float max = oneSemitoneChroma[0];
		for(int i=1; i<bps; i++){
			if(oneSemitoneChroma[i] > max){
				max = oneSemitoneChroma[i];
				whichBin = i;
			}
		}
		std::vector<float> twelveBpoChroma(hops*12*oct);
		for(int h = 0; h < hops; h++){
			for(int st = 0; st < 12*oct; st++){
				float weighted = 0;
				for(int b = 0; b < bps; b++)
					weighted += (getMagnitude(h,(st*bps)+b) * (b == whichBin ? 1.0 : prefs.getDetunedBandWeight()));
				twelveBpoChroma[(h*12*oct) + st] = weighted;
			}
		}
		chroma = twelveBpoChroma;
		bins = 12 * oct;
		return;
	}else if (method==2){
		// version of the above that tunes for each semitone bin
		// TODO build one that tunes adaptively over time?
		std::vector<float> twelveBpoChroma(hops*12*oct);
		for(int st = 0; st < 12*oct; st++){
			std::vector<float> oneSemitoneChroma(bps);
			for(int h = 0; h < hops; h++)
				for(int b = 0; b < bps; b++)
					oneSemitoneChroma[b] += getMagnitude(h,(st*bps)+b);
			// determine loudest tuning bin
			int whichBin = -1;
			float max = 0.0;
			for(int i=0; i<bps; i++){
				if(oneSemitoneChroma[i] > max){
					max = oneSemitoneChroma[i];
					whichBin = i;
				}
			}
			for(int h = 0; h < hops; h++){
				float weighted = 0;
				for(int b = 0; b < bps; b++)
					weighted += (getMagnitude(h,(st*bps)+b) * (b == whichBin ? 1.0 : prefs.getDetunedBandWeight()));
				twelveBpoChroma[(h*12*oct) + st] = weighted;
			}
		}
		chroma = twelveBpoChroma;
		bins = 12 * oct;
		return;
	}
}

void Chromagram::decomposeToOneOctave(const Preferences& prefs){
	int oct = prefs.getOctaves();
	int bpo = bins/oct;
	std::vector<float> oneOctaveChroma(hops*bpo);
	for(int h = 0; h < hops; h++){
		for(int b = 0; b < bpo; b++){
			float decomposedBin = 0.0;
			for(int o=0; o<oct; o++)
				decomposedBin += getMagnitude(h,o*bpo+b);
			oneOctaveChroma[h * bpo + b] = decomposedBin / oct;
		}
	}
	chroma = oneOctaveChroma;
	bins = bpo;
}

void Chromagram::setMagnitude(int h, int b, float val){
	if(h > hops){std::cerr << "setMagnitude hop " << h << " out of bounds" << std::endl; return;}
	if(b > bins){std::cerr << "setMagnitude bin " << b << " out of bounds" << std::endl; return;}
	chroma[(h * bins) + b] = val;
}

float Chromagram::getMagnitude(int h, int b) const{
	if(h > hops){std::cerr << "getMagnitude hop " << h << " out of bounds" << std::endl; return -1.0;}
	if(b > bins){std::cerr << "getMagnitude bin " << b << " out of bounds" << std::endl; return -1.0;}
	return chroma[(h * bins) + b];
}

void Chromagram::outputBinsByHops() const{
	std::cout << std::fixed;
	for(int h = 0; h < hops; h++){
		for(int b = 0; b < bins; b++){
			float out = getMagnitude(h,b);
			std::cout << std::setprecision(4) << out << "\t";
		}
		std::cout << std::endl;
	}
}

void Chromagram::outputHopsByBins() const{
	std::cout << std::fixed;
	for(int b = 0; b < bins; b++){
		for(int h = 0; h < hops; h++){
			float out = getMagnitude(h,b);
			std::cout << std::setprecision(4) << out << "\t";
		}
		std::cout << std::endl;
	}
}

int Chromagram::getHops() const{
	return hops;
}

int Chromagram::getBins() const{
	return bins;
}
