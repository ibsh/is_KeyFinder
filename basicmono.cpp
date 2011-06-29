#include "basicmono.h"

AudioBuffer* basicMono::makeMono(AudioBuffer* inbuf) throw (FatalException){
	int c = inbuf->audioChannels;
	if(c == 1)
		return inbuf;
	AudioBuffer* outbuf = new AudioBuffer();
	outbuf->frameRate = inbuf->frameRate;
	outbuf->audioChannels = 1;
	try{
		outbuf->addSamples(inbuf->audioSamples / c);
	}catch(FatalException){
		throw;
	}
	for(int i = 0; i < inbuf->audioSamples; i += c){
		float mono = 0.0;
		for(int j = 0; j < c; j++)
			mono += inbuf->getSample(i + j);
		mono /= c;
		outbuf->setSample(i/c,mono);
	}
	delete inbuf;
	return outbuf;
}

