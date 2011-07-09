#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <iostream>
#include <vector>

class AudioBuffer{
public:
	AudioBuffer();
	int getChannels() const;
	void setChannels(int);
	int getFrameRate() const;
	void setFrameRate(int);
	float getSample(int) const;
	void setSample(int,float);
	void addSamples(int);
	int getSampleCount() const;
	std::vector<float> buffer; // I don't want this public really, but libsndfile and libavcodec need access to buffer.front()
private:
	int channels;
	int frameRate;
	int samples;
};

#endif
