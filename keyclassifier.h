#ifndef KEYCLASSIFIER_H
#define KEYCLASSIFIER_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include "toneprofiles.h"
#include "preferences.h"

class KeyClassifier{
public:
	KeyClassifier(const Preferences&);
	~KeyClassifier();
	int classify(const std::vector<double>&);
private:
	ToneProfile* major;
	ToneProfile* minor;
};

#endif
