#include "chromagram.h"

using std::cout;
using std::fixed;
using std::setprecision;
using std::cerr;
using std::endl;

Chromagram::Chromagram(int h, int b){
	hops = h;
	bins = b;
	chroma = std::vector<float>(hops*bins);
}

// this version is more lightweight, and seems to perform better (x22 vs 23). Only used if bps > 1.
// TODO write a variation of this that tunes adaptively for each bin.
void Chromagram::decomposeToTwelveBpo(){
	Preferences* prefs = new Preferences(); // FIX
	int bpo = prefs->getBpo();
	int oct = prefs->getOctaves();
	int bps = (bpo/12); // bins per semitone
	std::vector<float> oneSemitoneChroma(bps);
	for(int h = 0; h < hops; h++)
		for(int st = 0; st < 12*oct; st++)
			for(int b = 0; b < bps; b++)
				oneSemitoneChroma[b] += getMagnitude(h,(st*bps)+b);
	// determine loudest tuning bin
	int whichBin = -1;
	float max = 0;
	for(int b = 0; b < bps; b++){
		if(oneSemitoneChroma[b] > max){
			whichBin = b;
			max = oneSemitoneChroma[b];
		}
	}
	std::vector<float> twelveBpoChroma(hops*12*oct);
	for(int h = 0; h < hops; h++){
		for(int st = 0; st < 12*oct; st++){
			float weighted = 0;
			for(int b = 0; b < bps; b++){
				if(b == whichBin)
					weighted += (1.0 * getMagnitude(h,(st*bps)+b));
				else
					weighted += (0.3 * getMagnitude(h,(st*bps)+b));
			}
			twelveBpoChroma[(h*12*oct) + st] = weighted;
		}
	}
	chroma = twelveBpoChroma;
	bins = 12 * oct;
}

/*
// this version is a lot more involved, and uses some weird approximation of Harte's method.
// also still uses C-style allocation
void Chromagram::decomposeToTwelveBpo(){
	Preferences* prefs = Preferences::Instance();
	int bpo = prefs->getBpo();
	int oct = prefs->getOctaves();
	int bps = bpo/12;
	// find peaks; anything that's higher energy than the mean for this hop and higher energy than its neighbours.
	vector<vector<float> > peakLocations;
	vector<vector<float> > peakMagnitudes;
	for(int hop = 0; hop < hops; hop++){
		// find mean magnitude for this hop
		float meanVal = 0;
		for(int bin = 0; bin < bins; bin++){
			meanVal += getMagnitude(hop,bin);
		}
		meanVal /= bins;
		// find peak bins
		vector<int> peakBins;
		for(int bin = 1; bin < bins-1; bin++){
			float binVal = getMagnitude(hop,bin);
			// currently every peak over mean. Tried all peaks but accuracy dropped.
			//if(binVal > getMagnitude(hop,bin-1) && binVal > getMagnitude(hop,bin+1)){
			if(binVal > meanVal && binVal > getMagnitude(hop,bin-1) && binVal > getMagnitude(hop,bin+1)){
				peakBins.push_back(bin);
			}
		}
		// apply quadratic interpolation to find a more precise peak position and magnitude.
		vector<float> peakLocationsRow;
		vector<float> peakMagnitudesRow;
		for(int peak=0; peak<peakBins.size(); peak++){
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
	vector<float> peakTuningDistribution(bps*10);
	for(int hop = 0; hop < hops; hop++){
		for(int peak = 0; peak < peakLocations[hop].size(); peak++){
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
	float* twelveBpoChroma = (float*)malloc(hops*12*oct*sizeof(float)); //TODO error checking
	for(int i=0; i<hops*12*oct; i++)
		twelveBpoChroma[i] = 0.0;
	// figure out which tuning bins to keep
	vector<int> binsToKeep;
	for(int i=(1-(bps*2)); i<bps*2; i++)
		binsToKeep.push_back((tuningPeak + i + (bps*10)) % (bps*10));
	// and discard the others
	for(int hop = 0; hop < hops; hop++){
		for(int peak = 0; peak < peakLocations[hop].size(); peak++){
			float peakLocationMod = fmodf(peakLocations[hop][peak],(float)bps);
			peakLocationMod *= 10;
			int peakLocationInt = peakLocationMod + 0.5;
			peakLocationInt += 5;
			bool discardMe = true;
			for(int i=0; i<binsToKeep.size(); i++)
				if(peakLocationInt == binsToKeep[i])
					discardMe = false;
			if(!discardMe){ // this is a valid peak for the tuned chromagram
				int tunedPeakLocation = (int)peakLocations[hop][peak];
				tunedPeakLocation /= bps;
				twelveBpoChroma[(hop*12*oct) + tunedPeakLocation] += peakMagnitudes[hop][peak];
			}
		}
	}
	free(chroma);
	chroma = twelveBpoChroma;
	bins = 12 * oct;
}
*/

void Chromagram::decomposeToOneOctave(){
	Preferences* prefs = new Preferences(); // FIX
	int oct = prefs->getOctaves();
	if(bins > 12 * oct) // TODO this shouldn't be automatic
		decomposeToTwelveBpo();
	std::vector<float> oneOctaveChroma(hops*12);
	for(int h = 0; h < hops; h++){
		for(int b = 0; b < 12; b++){
			float decomposedBin = 0;
			for(int o = 0; o < oct; o++){
				decomposedBin += getMagnitude(h,(o*12)+b);
			}
			oneOctaveChroma[(h * 12) + b] = decomposedBin / oct;
		}
	}
	chroma = oneOctaveChroma;
	bins = 12;
}

void Chromagram::setMagnitude(int h, int b, float val){
	if(h < 0 || h > hops) cerr << "setMagnitude hops (" << h << ") out of bounds" << endl;
	if(b < 0 || b > bins) cerr << "setMagnitude bins (" << b << ") out of bounds" << endl;
	chroma[(h * bins) + b] = val;
}

float Chromagram::getMagnitude(int h, int b) const{
	if(h < 0 || h > hops){cerr << "getMagnitude hops (" << h << ") out of bounds" << endl; return 0;}
	if(b < 0 || b > bins){cerr << "getMagnitude bins (" << b << ") out of bounds" << endl; return 0;}
	return chroma[(h * bins) + b];
}

void Chromagram::outputBinsByHops() const{
	int prec = 4; // TODO is this precision sensible?
	cout << fixed;
	for(int h = 0; h < hops; h++){
		for(int b = 0; b < bins; b++){
			float out = getMagnitude(h,b);
			cout << setprecision(prec) << out << "\t";
		}
		cout << endl;
	}
}

void Chromagram::outputHopsByBins() const{
	int prec = 4; // TODO is this precision sensible?
	cout << fixed;
	for(int b = 0; b < bins; b++){
		for(int h = 0; h < hops; h++){
			float out = getMagnitude(h,b);
			cout << setprecision(prec) << out << "\t";
		}
		cout << endl;
	}
}

int Chromagram::getHops() const{
	return hops;
}

int Chromagram::getBins() const{
	return bins;
}
