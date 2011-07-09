#ifndef IBDOWNSAMPLER_H
#define IBDOWNSAMPLER_H

#include "downsampler.h"

class IbDownsampler : public Downsampler{
public:
		virtual AudioBuffer* downsample(AudioBuffer*,int) throw (Exception);
};

#endif
