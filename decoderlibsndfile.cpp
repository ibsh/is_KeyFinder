#include "decoderlibsndfile.h"
#include <sndfile.h>

AudioBuffer* LibSndFileDecoder::decodeFile(char* filename) throw (FatalException){
	SNDFILE *soundFile;
	SF_INFO soundFileInfo;
	// Open the sound file.
	soundFileInfo.format = 0;
	soundFile = sf_open(filename,SFM_READ,&soundFileInfo);
	if (soundFile == NULL) throw FatalException("Failed to open audio file");
	AudioBuffer* ab = new AudioBuffer();
	// Get soundFileInfo
	ab->setFrameRate(soundFileInfo.samplerate);
	ab->setChannels(soundFileInfo.channels);
	try{
		ab->addSamples(soundFileInfo.frames * soundFileInfo.channels);
	}catch(FatalException){
		throw;
	}
	// Read PCM into buffer
	int audioSamplesRead = sf_read_float(soundFile, &ab->buffer.front(), ab->getSampleCount());
	if(audioSamplesRead < ab->getSampleCount()) throw FatalException("Failed to read all audio data");
	sf_close(soundFile);
	return ab;
}
