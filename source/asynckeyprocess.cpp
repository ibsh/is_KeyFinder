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

KeyFinderResultWrapper keyDetectionProcess(const AsyncFileObject& object) {

  KeyFinderResultWrapper result;
  result.batchRow = object.batchRow;

  AudioFileDecoder* decoder = NULL;
  try {

    decoder = new AudioFileDecoder(object.filePath, object.prefs.getMaxDuration());

  } catch (std::exception& e) {

    delete decoder;
    result.errorMessage = QString(e.what());
    return result;

  } catch (...) {

    delete decoder;
    result.errorMessage = "Unknown exception initialising decoder";
    return result;
  }

  KeyFinder::Workspace workspace;

  static KeyFinder::KeyFinder kf;

  try {

    while (true) {

      KeyFinder::AudioData* tempAudio = decoder->decodeNextAudioPacket();
      if (tempAudio == NULL) break;

      kf.progressiveChromagram(*tempAudio, workspace);
      delete tempAudio;
    }

    delete decoder;
    decoder = NULL;

    kf.finalChromagram(workspace);
    result.fullChromagram = KeyFinder::Chromagram(*workspace.chromagram);
    result.core = kf.keyOfChromagram(workspace);

  } catch (std::exception& e) {

    if (decoder != NULL) delete decoder;
    result.errorMessage = QString(e.what());
    return result;

  } catch (...) {

    if (decoder != NULL) {
      delete decoder;
      result.errorMessage = "Unknown exception while decoding";
    } else {
      result.errorMessage = "Unknown exception while analysing";
    }

    return result;
  }

  return result;
}
