#ifndef DISCARDDOWNSAMPLER_H
#define DISCARDDOWNSAMPLER_H

#include "downsampler.h"

class DiscardDownsampler : public Downsampler{
public:
		virtual AudioBuffer* downsample(AudioBuffer*,int) throw (FatalException);
};

#endif
