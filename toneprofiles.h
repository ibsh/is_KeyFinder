#ifndef TONEPROFILES_H
#define TONEPROFILES_H

#include <vector>
#include <math.h>
#include <stdlib.h>

class ToneProfile{
public:
	ToneProfile(int,bool);
	~ToneProfile();
	double correlate(const std::vector<double>&,const double&, const int&);
	double cosine(const std::vector<double>&, const int&);
private:
	void free();
	class Binode;
	Binode* tonic;
	double mean;
};

class ToneProfile::Binode{
public:
	Binode(double x): n(x), l(NULL), r(NULL){}
	double n;
	Binode *l, *r;
};

#endif
