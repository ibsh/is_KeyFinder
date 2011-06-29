#ifndef WINDOWFUNCTIONS_H
#define WINDOWFUNCTIONS_H

#include "preferences.h"
#include <math.h>
#include <stdlib.h>

class WindowFunction{
public:
	WindowFunction();
	virtual float window(int,int) const = 0;
protected:
	float pi;
};

class HannWindow : public WindowFunction{
public:
	virtual float window(int,int) const;
};

class HammingWindow : public WindowFunction{
public:
	virtual float window(int,int) const;
};

class BlackmanWindow : public WindowFunction{
public:
	virtual float window(int,int) const;
};

#endif
