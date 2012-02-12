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

#include "audiostream.h"

AudioStream::AudioStream():  stream(0), channels(0), frameRate(0),  samples(0){}

int AudioStream::getChannels() const{
  return channels;
}

void AudioStream::setChannels(int n){
  channels = n;
}

int AudioStream::getFrameRate() const{
  return frameRate;
}

void AudioStream::setFrameRate(int n){
  frameRate = n;
}

float AudioStream::getSample(int n) const{
  if(n < samples){
    return stream[n];
  }else{
    qDebug("Attempt to get out-of-bounds sample (%d/%d)",n,samples);
    return 0;
  }
}

void AudioStream::setSample(int n,float x){
  if(n < samples)
    stream[n] = x;
  else
    qDebug("Attempt to set out-of-bounds sample (%d/%d)",n,samples);
}

void AudioStream::addToSampleCount(int newSamples){
  try{
    stream.resize(samples + newSamples);
    samples += newSamples;
  }catch(...){
    qCritical("Memory allocation failure adding %d samples to audio stream of size %d",newSamples,samples);
    throw Exception();
  }
}

int AudioStream::getSampleCount() const{
  return samples;
}

void AudioStream::reduceToMono(){
  if(channels == 1) return;
  std::vector<float> newStream(samples / channels);
  for(int i = 0; i < samples; i += channels){
    float mono = 0.0;
    for(int j = 0; j < channels; j++)
      mono += stream[i + j];
    mono /= channels;
    newStream[i/channels] = mono;
  }
  stream = newStream;
  samples /= channels;
  channels = 1;
  return;
}
