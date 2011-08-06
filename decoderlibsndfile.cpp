#include "decoderlibsndfile.h"
#include <sndfile.h>

AudioBuffer* LibSndFileDecoder::decodeFile(char* fileName) throw (Exception){
	SNDFILE *soundFile;
	SF_INFO soundFileInfo;
	// Open the sound file.
	soundFileInfo.format = 0;
	soundFile = sf_open(fileName,SFM_READ,&soundFileInfo);
	if (soundFile == NULL){
		qCritical("Failed to open audio file: %s", fileName);
		throw Exception();
	}
	AudioBuffer* ab = new AudioBuffer();
	// Get soundFileInfo
	ab->setFrameRate(soundFileInfo.samplerate);
	ab->setChannels(soundFileInfo.channels);
	try{
		ab->addSamples(soundFileInfo.frames * soundFileInfo.channels);
	}catch(const Exception& e){
		throw e;
	}
	// Read PCM into buffer
	int audioSamplesRead = sf_read_float(soundFile, &ab->buffer.front(), ab->getSampleCount());
	if(audioSamplesRead < ab->getSampleCount()){
		qCritical("Failed to read all audio data in file: %s", fileName);
		throw Exception();
	}
	sf_close(soundFile);
	return ab;
}
