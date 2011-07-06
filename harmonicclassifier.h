#ifndef HARMONICCLASSIFIER_H
#define HARMONICCLASSIFIER_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include "toneprofiles.h"
#include "preferences.h"

class HarmonicClassifier{
public:
	HarmonicClassifier(int);
	~HarmonicClassifier();
	int classify(const std::vector<double>&, const Preferences&);
private:
	ToneProfile* major;
	ToneProfile* minor;
};

#endif
