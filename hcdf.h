#ifndef HDCF_H
#define HDCF_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <math.h>
#include <stdlib.h>
#include "chromagram.h"
#include "preferences.h"

class Hcdf{
public:
	Hcdf();
	std::vector<int> hcdf(Chromagram*, const Preferences&);
private:
	double phi(int, int);
	double pi;
};

#endif
