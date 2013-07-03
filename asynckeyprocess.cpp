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

  AudioFileDecoder* decoder = NULL;
  try{
    decoder = new AudioFileDecoder(object.filePath, object.prefs.getMaxDuration());
  }catch(std::exception& e){
    delete decoder;
    result.errorMessage = QString(e.what());
    return result;
  }catch(...){
    delete decoder;
    result.errorMessage = "Unknown exception initialising decoder";
    return result;
  }

  KeyFinder::Workspace workspace;
  KeyFinder::KeyFinder* kf = LibKeyFinderSingleton::getInstance()->getKeyFinder();
  try{
    while(true){
      KeyFinder::AudioData* tempAudio = decoder->decodeFile();
      if(tempAudio == NULL) break;
      kf->progressiveChromagram(*tempAudio, workspace, object.prefs.core);
      delete tempAudio;
    }
    delete decoder;
    decoder = NULL;
    kf->finalChromagram(workspace, object.prefs.core);
    result.fullChromagram = KeyFinder::Chromagram(*workspace.chromagram);
    result.core = kf->keyOfChromagram(workspace, object.prefs.core);
    result.oneOctaveChromagram = result.fullChromagram;
    result.oneOctaveChromagram.reduceToOneOctave();
  }catch(std::exception& e){
    if (decoder != NULL) delete decoder;
    result.errorMessage = QString(e.what());
    return result;
  }catch(...){
    if (decoder != NULL) {
      delete decoder;
      result.errorMessage = "Unknown exception while decoding";
    } else {
      result.errorMessage = "Unknown exception while analysing";
    }
    return result;
  }

  for (unsigned int i = 0; i < result.core.segments.size(); i++) {
    qDebug(
      "Chroma vector for segment %d of file %s: [%.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f, %.0f]",
      i, object.filePath.toLocal8Bit().constData(),
      result.core.segments[i].chromaVector[0],  result.core.segments[i].chromaVector[1],
      result.core.segments[i].chromaVector[2],  result.core.segments[i].chromaVector[3],
      result.core.segments[i].chromaVector[4],  result.core.segments[i].chromaVector[5],
      result.core.segments[i].chromaVector[6],  result.core.segments[i].chromaVector[7],
      result.core.segments[i].chromaVector[8],  result.core.segments[i].chromaVector[9],
      result.core.segments[i].chromaVector[10], result.core.segments[i].chromaVector[11]
    );
  }

  return result;
}
