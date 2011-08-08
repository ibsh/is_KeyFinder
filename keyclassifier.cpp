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

#include "keyclassifier.h"

KeyClassifier::KeyClassifier(const Preferences& prefs){
	// Profiles
	major = new ToneProfile(prefs.getToneProfile(),true,prefs);
	minor = new ToneProfile(prefs.getToneProfile(),false,prefs);
	silence = new ToneProfile(-1,true,prefs);
}

KeyClassifier::~KeyClassifier(){
	delete major;
	delete minor;
	delete silence;
}

int KeyClassifier::classify(const std::vector<double>& chroma){
	std::vector<double> scores(24);
	for(int i=0; i<12; i++){ // for each pair of profiles
		double cosi = major->cosine(chroma,i); // major
		scores[i*2] = cosi;
		cosi = minor->cosine(chroma,i); // minor
		scores[(i*2)+1] = cosi;
	}
	// find best match, starting with silence
	double bestScore = silence->cosine(chroma,0);
	int bestMatch = 24;
	for(int i=0; i<24; i++){
		if(scores[i] > bestScore){
			bestScore = scores[i];
			bestMatch = i;
		}
	}
	return bestMatch;
}
