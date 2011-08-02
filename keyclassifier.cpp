#include "keyclassifier.h"

KeyClassifier::KeyClassifier(const Preferences& prefs){
	// Profiles
	major = new ToneProfile(prefs.getToneProfile(),true,prefs);
	minor = new ToneProfile(prefs.getToneProfile(),false,prefs);
}

KeyClassifier::~KeyClassifier(){
	delete major;
	delete minor;
}

int KeyClassifier::classify(const std::vector<double>& chroma){
	std::vector<double> scores(24);
	for(int i=0; i<12; i++){ // for each pair of profiles
		double cosi = major->cosine(chroma,i); // major
		scores[i*2] = cosi;
		cosi = minor->cosine(chroma,i); // minor
		scores[(i*2)+1] = cosi;
	}
	// find best match
	int bestMatch = 0; // this does mean that absolute silence will be visually marked as A major. But it will be weighted zero.
	double bestScore = scores[0];
	for(int i=1; i<24; i++){
		if(scores[i] > bestScore){
			bestScore = scores[i];
			bestMatch = i;
		}
	}
	return bestMatch;
}
