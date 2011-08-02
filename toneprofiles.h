#ifndef TONEPROFILES_H
#define TONEPROFILES_H

#include <preferences.h>

#include <vector>
#include <math.h>
#include <stdlib.h>

class ToneProfile{
public:
	ToneProfile(int,bool,const Preferences&);
	~ToneProfile();
	double cosine(const std::vector<double>&, const int&) const;
private:
	void free();
	class Binode;
	Binode* tonic;
};

class ToneProfile::Binode{
public:
	Binode(double x): n(x), l(NULL), r(NULL){}
	double n;
	Binode *l, *r;
};

#endif
