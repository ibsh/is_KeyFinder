#include "basicmono.h"

AudioBuffer* basicMono::makeMono(AudioBuffer* inbuf) throw (FatalException){
	int c = inbuf->getChannels();
	if(c == 1)
		return inbuf;
	AudioBuffer* outbuf = new AudioBuffer();
	outbuf->setFrameRate(inbuf->getFrameRate());
	outbuf->setChannels(1);
	try{
		outbuf->addSamples(inbuf->getSampleCount() / c);
	}catch(FatalException){
		throw;
	}
	for(int i = 0; i < inbuf->getSampleCount(); i += c){
		float mono = 0.0;
		for(int j = 0; j < c; j++)
			mono += inbuf->getSample(i + j);
		mono /= c;
		outbuf->setSample(i/c,mono);
	}
	delete inbuf;
	return outbuf;
}

