#include "toneprofiles.h"

ToneProfile::ToneProfile(int whichProfile, bool majorScale, const Preferences& prefs){
	double p[12];
	if(whichProfile==1){ // Temperley profiles
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
	}else if(whichProfile==2){ // Sha'ath profiles
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
	Determines cosine similarity between input vector and a particular scale.
	input = array of 12 doubles relating to an octave starting at A natural
	offset = which scale to test against; 0 = A, 1 = Bb, 2 = B, 3 = C etc
*/
double ToneProfile::cosine(const std::vector<double>& input, const int& offset) const{
	// Rotate starting pointer left for offset. Each step shifts the position
	// of the tonic one step further right of the starting pointer (or one semitone up).
	Binode* p = tonic;
	for(int i=0; i<offset; i++)
		p = p->l;
	// calculate cosine similarity between vector and offset circular list
	double top = 0.0;
	double bottomleft = 0.0;
	double bottomright = 0.0;
	for(int i=0; i<12; i++){
		top += input[i] * p->n;
		bottomleft += pow((p->n),2);
		bottomright += pow((input[i]),2);
		p = p->r;
	}
	if(bottomleft > 0 && bottomright > 0) // divzero
		return top / (sqrt(bottomleft) * sqrt(bottomright));
	else
		return 0;
}
