#include "toneprofiles.h"

ToneProfile::ToneProfile(int whichProfile = 0, bool majorScale = false){
	// whichProfile determines whose profiles to use
	// majorScale determines major or minor
	double p[12];
	if(whichProfile==1){ // Temperley profiles
		if(majorScale){
			p[0]=5.0; p[1]=2.0; p[2]=3.5;
			p[3]=2.0; p[4]=4.5; p[5]=4.0;
			p[6]=2.0; p[7]=4.5; p[8]=2.0;
			p[9]=3.5; p[10]=1.5; p[11]=4.0;
		}else{
			p[0]=5.0; p[1]=2.0; p[2]=3.5;
			p[3]=4.5; p[4]=2.0; p[5]=4.0;
			p[6]=2.0; p[7]=4.5; p[8]=3.5;
			p[9]=2.0; p[10]=1.5; p[11]=4.0;
		}
	}else if(whichProfile==2){ // Sha'ath profiles
		if(majorScale){
			p[0]=7.0; p[1]=2.0; p[2]=3.5;
			p[3]=2.0; p[4]=4.5; p[5]=4.0;
			p[6]=2.0; p[7]=4.5; p[8]=2.0;
			p[9]=3.5; p[10]=1.5; p[11]=4.0;
		}else{
			p[0]=7.0; p[1]=2.0; p[2]=3.5;
			p[3]=4.5; p[4]=2.0; p[5]=4.0;
			p[6]=2.0; p[7]=4.5; p[8]=3.5;
			p[9]=2.0; p[10]=4.0; p[11]=1.5;
		}
	}else{ // Krumhansl profiles by default
		if(majorScale){
			p[0]=6.35; p[1]=2.23; p[2]=3.48;
			p[3]=2.33; p[4]=4.38; p[5]=4.09;
			p[6]=2.52; p[7]=5.19; p[8]=2.39;
			p[9]=3.66; p[10]=2.29; p[11]=2.88;
		}else{
			p[0]=6.33; p[1]=2.68; p[2]=3.52;
			p[3]=5.38; p[4]=2.60; p[5]=3.53;
			p[6]=2.54; p[7]=4.75; p[8]=3.98;
			p[9]=2.69; p[10]=3.34; p[11]=3.17;
		}
	}
	for(int i=0; i<12; i++)
		mean += p[i];
	mean /= 12;
	tonic = new Binode(p[0]);
	Binode *q = tonic;
	for(int i=1; i<12; i++){
		q->r = new Binode(p[i]);
		q->r->l = q;
		q = q->r;
	}
	q->r = tonic;
	tonic->l = q;
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

double ToneProfile::correlate(const std::vector<double>& input, const double& inputMean, const int& offset){
	// input = array of 12 doubles relating to an octave starting at A natural
	// inputMean = precalculated mean of input's values
	// offset = which scale to test against; 0 = A, 1 = Bb, 2 = B, 3 = C etc
	double sum1 = 0.0;
	double sum2 = 0.0;
	double sum3 = 0.0;
	Binode* p = tonic;
	for(int i=0; i<offset; i++)
		p = p->l; // going left. i.e. each step will be shifting the comparison of the tonic one semitone right (up)
	for(int i=0; i<12; i++){ // for each note in whichProfile
		sum1 += (p->n - mean) * (input[i] - inputMean);
		sum2 += pow((p->n - mean),2);
		sum3 += pow((input[i] - inputMean),2);
		p = p->r;
	}
	if(sum3 > 0 && sum2 > 0) // divzero
		return sum1 / sqrt(sum2 * sum3);
	else
		return 0;
}

double ToneProfile::cosine(const std::vector<double>& input, const int& offset){
	// input = array of 12 doubles relating to an octave starting at A natural
	// offset = which scale to test against; 0 = A, 1 = Bb, 2 = B, 3 = C etc
	double top = 0.0;
	double bottomleft = 0.0;
	double bottomright = 0.0;
	Binode* p = tonic;
	for(int i=0; i<offset; i++)
		p = p->l; // going left. i.e. each step will be shifting the comparison of the tonic one semitone right (up)
	for(int i=0; i<12; i++){ // for each note in whichProfile
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
