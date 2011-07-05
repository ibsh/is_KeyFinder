#include "audiobuffer.h"

AudioBuffer::AudioBuffer(): audioSamples(0), audioChannels(0), frameRate(0), currentSample(0), buffer(0){}

void AudioBuffer::addSamples(int newSamples) throw (FatalException){
	//std::cerr << "Adding " << newSamples << " to " << audioSamples << " for " << audioSamples + newSamples << " total." << std::endl;
	buffer.resize(audioSamples + newSamples);
	audioSamples += newSamples;
}

float AudioBuffer::getSample(int n) const{
	if(n >= 0 && n < audioSamples){
		return buffer[n];
	}else{
		std::cerr << "Attempt to get out-of-bounds sample (" << n << "/" << audioSamples << ")" << std::endl;
		return 0;
	}
}

void AudioBuffer::setSample(int n,float x){
	if(n >= 0 && n < audioSamples)
		buffer[n] = x;
	else
		std::cerr << "Attempt to set out-of-bounds sample (" << n << "/" << audioSamples << ")" << std::endl;
}
