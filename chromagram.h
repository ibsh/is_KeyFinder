#ifndef CHROMAGRAM_H
#define CHROMAGRAM_H

#include "preferences.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>
#include <stdlib.h>

class Chromagram{
public:
	Chromagram(int,int);
	void setMagnitude(int,int,float);
	float getMagnitude(int,int) const;
	int getHops() const;
	int getBins() const;
	void decomposeToOneOctave();
	void decomposeToTwelveBpo();
	void outputBinsByHops() const;
	void outputHopsByBins() const;
private:
	int hops;
	int bins;
	std::vector<float> chroma;
};

#endif
