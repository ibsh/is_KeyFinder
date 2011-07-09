#ifndef SECRETRABBITDOWNSAMPLER_H
#define SECRETRABBITDOWNSAMPLER_H

#include "downsampler.h"
#include <sstream>

class SecretRabbitDownsampler : public Downsampler{
public:
	virtual AudioBuffer* downsample(AudioBuffer*,int) throw (Exception);
};

#endif
