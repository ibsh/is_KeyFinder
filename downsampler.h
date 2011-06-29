#ifndef DOWNSAMPLER_H
#define DOWNSAMPLER_H

#include "fatalexception.h"
#include "audiobuffer.h"

class Downsampler{
public:
	virtual AudioBuffer* downsample(AudioBuffer*,int) throw (FatalException) = 0;
};

#endif
