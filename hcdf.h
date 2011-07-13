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
	std::vector<double> hcdf(Chromagram*, const Preferences&);
	std::vector<int> peaks(const std::vector<double>&, const Preferences&);
private:
	double phi(int, int);
	double pi;
};

#endif
