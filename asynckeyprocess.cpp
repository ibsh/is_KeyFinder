/*************************************************************************

  Copyright 2011-2013 Ibrahim Sha'ath

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
  }catch(std::exception& e){
    delete decoder;
    result.errorMessage = QString(e.what());
    return result;
  }catch(...){
    delete audio;
    result.errorMessage = "Unknown exception initialising decoder";
    return result;
  }

  try{
    audio = decoder->decodeFile(object.filePath, object.prefs.getMaxDuration());
    delete decoder;
  }catch(std::exception& e){
    delete audio;
    delete decoder;
    result.errorMessage = QString(e.what());
    return result;
  }catch(...){
    delete audio;
    result.errorMessage = "Unknown exception from decoder";
    return result;
  }

  KeyFinder::KeyFinder* kf = LibKeyFinderSingleton::getInstance()->getKeyFinder();
  try{
    result.core = kf->findKey(*audio, object.prefs.core);
  }catch(const std::exception& e){
    delete audio;
    result.errorMessage = QString(e.what());
    return result;
  }catch(...){
    delete audio;
    result.errorMessage = "Unknown exception from libKeyFinder";
    return result;
  }


  delete audio;

  return result;
}
