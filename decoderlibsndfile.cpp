/*************************************************************************

	Copyright 2011 Ibrahim Sha'ath

	This file is part of KeyFinder.

	KeyFinder is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	KeyFinder is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with KeyFinder.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

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
