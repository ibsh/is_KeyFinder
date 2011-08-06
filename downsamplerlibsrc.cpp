#include "downsamplerlibsrc.h"
#include <samplerate.h>

AudioBuffer* SecretRabbitDownsampler::downsample(AudioBuffer* inbuf, int factor) throw (Exception){
	if(factor == 1) return inbuf;
	AudioBuffer* outbuf = new AudioBuffer();
	outbuf->setFrameRate(inbuf->getFrameRate() / factor);
	outbuf->setChannels(inbuf->getChannels());
	try{
		outbuf->addSamples(inbuf->getSampleCount() / factor);
	}catch(const Exception& e){
		throw e;
	}
	SRC_DATA srcData;
	srcData.data_in = &inbuf->buffer.front();
	srcData.data_out = &outbuf->buffer.front();
	srcData.input_frames = (long)(inbuf->getSampleCount() / inbuf->getChannels());
	srcData.output_frames = (long)(outbuf->getSampleCount() / outbuf->getChannels());
	srcData.src_ratio = 1.0/factor;
	// second parameter = converter type.
	// 0-2 quite slow but they anti-alias roughly the same amount, based on a visual inspection.
	// 3-4 super fast but the aliasing is worse than a simple discard. Don't use.
	int result = src_simple(&srcData,2,inbuf->getChannels());
	if(result != 0){
		qCritical("Error in LibSRC sample rate conversion");
		throw Exception();
	}
	delete inbuf;
	return outbuf;
}
