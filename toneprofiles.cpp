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

#include "toneprofiles.h"

ToneProfile::ToneProfile(int whichProfile, bool majorScale, const Preferences& prefs){
	double p[12];
	if(whichProfile == -1){ // Silence profile
		p[0]=0; p[1]=0;
		p[2]=0; p[3]=0;
		p[4]=0; p[5]=0;
		p[6]=0; p[7]=0;
		p[8]=0; p[9]=0;
		p[10]=0; p[11]=0;
	}else if(whichProfile == 1){ // Temperley profiles
		if(majorScale){
			p[0]=5.0; p[1]=2.0;
			p[2]=3.5; p[3]=2.0;
			p[4]=4.5;
			p[5]=4.0; p[6]=2.0;
			p[7]=4.5; p[8]=2.0;
			p[9]=3.5; p[10]=1.5;
			p[11]=4.0;
		}else{
			p[0]=5.0; p[1]=2.0;
			p[2]=3.5;
			p[3]=4.5; p[4]=2.0;
			p[5]=4.0; p[6]=2.0;
			p[7]=4.5;
			p[8]=3.5; p[9]=2.0;
			p[10]=1.5; p[11]=4.0;
		}
	}else if(whichProfile == 2){ // Gomez profiles
		if(majorScale){
			p[0]=0.82; p[1]=0.00;
			p[2]=0.55; p[3]=0.00;
			p[4]=0.53;
			p[5]=0.30; p[6]=0.08;
			p[7]=1.00; p[8]=0.00;
			p[9]=0.38; p[10]=0.00;
			p[11]=0.47;
		}else{
			p[0]=0.81; p[1]=0.00;
			p[2]=0.53;
			p[3]=0.54; p[4]=0.00;
			p[5]=0.27; p[6]=0.07;
			p[7]=1.00;
			p[8]=0.27; p[9]=0.07;
			p[10]=0.10; p[11]=0.36;
		}
	}else if(whichProfile == 3){ // Sha'ath profiles
		if(majorScale){
			p[0]=6.6; p[1]=2.0;
			p[2]=3.5; p[3]=2.3;
			p[4]=4.6;
			p[5]=4.0; p[6]=2.5;
			p[7]=5.2; p[8]=2.4;
			p[9]=3.7; p[10]=2.3;
			p[11]=3.4;
		}else{
			p[0]=6.5; p[1]=2.7;
			p[2]=3.5;
			p[3]=5.4; p[4]=2.6;
			p[5]=3.5; p[6]=2.5;
			p[7]=5.2;
			p[8]=4.0; p[9]=2.7;
			p[10]=4.3; p[11]=3.2;
		}
	}else if(whichProfile == 4){ // Custom profiles
		std::vector<float> ctp = prefs.getCustomToneProfile();
		if(majorScale){
			for(int i=0; i<12; i++)
				p[i] = ctp[i];
		}else{
			for(int i=0; i<12; i++)
				p[i] = ctp[i+12];
		}
	}else{ // Krumhansl profiles
		if(majorScale){
			p[0]=6.35; p[1]=2.23;
			p[2]=3.48; p[3]=2.33;
			p[4]=4.38;
			p[5]=4.09; p[6]=2.52;
			p[7]=5.19; p[8]=2.39;
			p[9]=3.66; p[10]=2.29;
			p[11]=2.88;
		}else{
			p[0]=6.33; p[1]=2.68;
			p[2]=3.52;
			p[3]=5.38; p[4]=2.60;
			p[5]=3.53; p[6]=2.54;
			p[7]=4.75;
			p[8]=3.98; p[9]=2.69;
			p[10]=3.34; p[11]=3.17;
		}
	}
	tonic = new Binode(p[0]);
	Binode *q = tonic;
	for(int i=1; i<12; i++){
		q->r = new Binode(p[i]);
		q->r->l = q;
		q = q->r;
	}
	q->r = tonic;
	tonic->l = q;
	// offset by number of semitones specified in preferences
	for(int i=0; i<prefs.getOctaveOffset(); i++)
		tonic = tonic->r;
	// get mean in preparation for correlation
	profileMean = 0.0;
	for(int i=0; i<12; i++)
		profileMean += (p[i] / 12.0);
}

ToneProfile::~ToneProfile(){
	free();
}

void ToneProfile::free(){
	Binode* p = tonic;
	do{
		Binode* zap = p;
		p = p->r;
		delete zap;
	}while(p!=tonic);
}

/*
	Determines cosine similarity between input vector and profile scale.
	input = array of 12 doubles relating to an octave starting at A natural
	offset = which scale to test against; 0 = A, 1 = Bb, 2 = B, 3 = C etc
*/
double ToneProfile::cosine(const std::vector<double>& input, const int& offset) const{
	// Rotate starting pointer left for offset. Each step shifts the position
	// of the tonic one step further right of the starting pointer (or one semitone up).
	Binode* p = tonic;
	for(int i=0; i<offset; i++)
		p = p->l;
	double intersection = 0.0;
	double profileNorm = 0.0;
	double inputNorm = 0.0;
	for(int i=0; i<12; i++){
		intersection += input[i] * p->n;
		profileNorm += pow((p->n),2);
		inputNorm += pow((input[i]),2);
		p = p->r;
	}
	if(profileNorm > 0 && inputNorm > 0) // divzero
		return intersection / (sqrt(profileNorm) * sqrt(inputNorm));
	else
		return 0;
}

/*
	Krumhansl's correlation between input vector and profile scale.
	input = array of 12 doubles relating to an octave starting at A natural
	offset = which scale to test against; 0 = A, 1 = Bb, 2 = B, 3 = C etc
*/
double ToneProfile::correlation(const std::vector<double>& input, const double& inputMean, const int& offset) const{
	Binode* p = tonic;
	for(int i=0; i<offset; i++)
		p = p->l;
	double sumTop = 0.0;
	double sumBottomLeft = 0.0;
	double sumBottomRight = 0.0;
	for(int i=0; i<12; i++){
		double xMinusXBar = p->n - profileMean;
		double yMinusYBar = input[i] - inputMean;
		sumTop += xMinusXBar * yMinusYBar;
		sumBottomLeft += pow(xMinusXBar,2);
		sumBottomRight += pow(yMinusYBar,2);
		p = p->r;
	}
	if(sumBottomRight > 0 && sumBottomLeft > 0) // divzero
		return sumTop / sqrt(sumBottomLeft * sumBottomRight);
	else
		return 0;
}
