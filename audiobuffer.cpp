#include "audiobuffer.h"

AudioBuffer::AudioBuffer(): samples(0), channels(0), frameRate(0), buffer(0){}

int AudioBuffer::getChannels() const{
	return channels;
}

void AudioBuffer::setChannels(int n){
	channels = n;
}

int AudioBuffer::getFrameRate() const{
	return frameRate;
}

void AudioBuffer::setFrameRate(int n){
	frameRate = n;
}

float AudioBuffer::getSample(int n) const{
	if(n >= 0 && n < samples){
		return buffer[n];
	}else{
		std::cerr << "Attempt to get out-of-bounds sample (" << n << "/" << samples << ")" << std::endl;
		return 0;
	}
}

void AudioBuffer::setSample(int n,float x){
	if(n >= 0 && n < samples)
		buffer[n] = x;
	else
		std::cerr << "Attempt to set out-of-bounds sample (" << n << "/" << samples << ")" << std::endl;
}

void AudioBuffer::addSamples(int newSamples){
	//std::cerr << "Adding " << newSamples << " to " << audioSamples << " for " << audioSamples + newSamples << " total." << std::endl;
	buffer.resize(samples + newSamples);
	samples += newSamples;
}

int AudioBuffer::getSampleCount() const{
	return samples;
}
