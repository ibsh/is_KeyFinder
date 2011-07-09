#include "downsampler.h"

#include "downsamplerib.h"
#include "downsamplerlibsrc.h"

Downsampler* Downsampler::getDownsampler(int downsampleFactor, int frameRate, float maxFreq){
	if(downsampleFactor == 10 && frameRate == 44100 && maxFreq < 2205.0){
		return new IbDownsampler();
	}else{
		return new SecretRabbitDownsampler();
	}
}
