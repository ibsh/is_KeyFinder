#ifndef DOWNSAMPLER_H
#define DOWNSAMPLER_H

#include "exception.h"
#include "audiobuffer.h"

class Downsampler{
public:
	virtual AudioBuffer* downsample(AudioBuffer*,int) throw (Exception) = 0;
	static Downsampler* getDownsampler(int,int,float);
};

#endif
