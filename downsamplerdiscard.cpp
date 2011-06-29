#include "downsamplerdiscard.h"

AudioBuffer* DiscardDownsampler::downsample(AudioBuffer* inbuf, int factor) throw (FatalException){
	if(factor == 1) return inbuf;
	AudioBuffer* outbuf = new AudioBuffer();
	outbuf->frameRate = inbuf->frameRate / factor;
	int c = outbuf->audioChannels = inbuf->audioChannels;
	int ns = inbuf->audioSamples / factor;
	while(ns%c != 0) ns++;
	if(inbuf->audioSamples % factor > 0) ns += c;
	try{
		outbuf->addSamples(ns);
	}catch(FatalException){
		throw;
	}
	for(int i = 0; i < inbuf->audioSamples; i += (factor * c)){ // for each frame
		for(int j = 0; j < c; j++){ // for each sample in frame
			outbuf->setSample((i/factor)+j,inbuf->getSample(i + j));
		}
	}
	delete inbuf;
	return outbuf;
}
