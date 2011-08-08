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

#include "audiobuffer.h"

AudioBuffer::AudioBuffer():  buffer(0), channels(0), frameRate(0),  samples(0){}

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
	if(n < samples){
		return buffer[n];
	}else{
		qDebug("Attempt to get out-of-bounds sample (%d/%d)",n,samples);
		return 0;
	}
}

void AudioBuffer::setSample(int n,float x){
	if(n < samples)
		buffer[n] = x;
	else
		qDebug("Attempt to set out-of-bounds sample (%d/%d)",n,samples);
}

void AudioBuffer::addSamples(int newSamples) throw (Exception){
	try{
		buffer.resize(samples + newSamples);
		samples += newSamples;
	}catch(...){
		qCritical("Memory allocation failure adding %d samples to buffer of size %d",newSamples,samples);
		throw Exception();
	}
}

int AudioBuffer::getSampleCount() const{
	return samples;
}
