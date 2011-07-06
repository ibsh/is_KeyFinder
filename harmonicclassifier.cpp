#include "harmonicclassifier.h"

HarmonicClassifier::HarmonicClassifier(int whichProfile){
	// Profiles
	major = new ToneProfile(whichProfile,true);
	minor = new ToneProfile(whichProfile,false);
}

HarmonicClassifier::~HarmonicClassifier(){
	delete major;
	delete minor;
}

int HarmonicClassifier::classify(const std::vector<double>& chroma, const Preferences& prefs){
	std::vector<double> scores(24);
	// note 12 magic number below. not ideal, but using prefs.getBpo breaks for > 1bps.
	for(int i=0; i<12; i++){ // for each pair of profiles
		double cosi = major->cosine(chroma,i); // major
		scores[i*2] = cosi;
		cosi = minor->cosine(chroma,i); // minor
		scores[(i*2)+1] = cosi;
	}
	// find best match
	int bestMatch = -1;
	double bestScore = 0;
	for(int i=0; i<24; i++){
		if(scores[i] > bestScore){
			bestScore = scores[i];
			bestMatch = i;
		}
	}
	return bestMatch;
}
