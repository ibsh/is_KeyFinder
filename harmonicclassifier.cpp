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

int HarmonicClassifier::classify(const std::vector<double>& chroma){
	Preferences* prefs = new Preferences(); // FIX
	std::vector<double> scores(24);
	for(int i=0; i<prefs->getBpo(); i++){ // for each pair of profiles
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
