#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include "fatalexception.h"
#include <stdlib.h>
#include <iostream>
#include <vector>

class AudioBuffer{
public:
	AudioBuffer();
	std::vector<float> buffer; // TODO don't want this public really
	int audioSamples; // TODO public getter, no setter
	int currentSample; // TODO this is a crappy implementation
	int audioChannels;
	int frameRate;
	void addSamples(int) throw (FatalException);
	float getSample(int) const;
	void setSample(int,float);
};

#endif
