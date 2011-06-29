#ifndef HDCFHARTE_H
#define HDCFHARTE_H

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

class HarteHCDF{
public:
	HarteHCDF();
	std::vector<int> hcdf(Chromagram*);
private:
	double phi(int, int);
	double pi;
};

#endif
