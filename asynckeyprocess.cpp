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

#include "asynckeyprocess.h"

KeyFinderResultWrapper keyDetectionProcess(const AsyncFileObject& object){

  KeyFinderResultWrapper result;
  result.batchRow = object.batchRow;

  // initialise stream and decode file into it.
  KeyFinder::AudioData* audio = NULL;
  AudioFileDecoder* decoder = NULL;
  try{
    decoder = AudioFileDecoder::getDecoder();
  }catch(Exception){
    delete decoder;
    result.errorMessage = "Could not get decoder.";
    return result;
  }

  try{
    audio = decoder->decodeFile(object.filePath);
    delete decoder;
  }catch(Exception){
    delete audio;
    delete decoder;
    result.errorMessage = "Could not decode file.";
    return result;
  }

  // make audio stream monaural ahead of downsample to reduce load
  audio->reduceToMono();

  // downsample if necessary
  if(object.prefs.getDFactor() > 1){
    Downsampler* ds = Downsampler::getDownsampler(object.prefs.getDFactor(),audio->getFrameRate(),object.prefs.getLastFreq());
    try{
      audio = ds->downsample(audio,object.prefs.getDFactor());
    }catch(Exception){
      delete audio;
      delete ds;
      result.errorMessage = "Downsampler failed.";
      return result;
    }
    delete ds;
  }

  KeyFinder::KeyFinder keyFinder;
  result.core = keyFinder.findKey(audio, object.prefs.core);

  return result;
}
