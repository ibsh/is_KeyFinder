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

KeyDetectionResult keyDetectionProcess(const AsyncFileObject& object){

  KeyDetectionResult result;
  result.batchRow = object.batchRow;

  qDebug("Analysing %s", object.filePath.toLocal8Bit().data());

  // initialise stream and decode file into it.
  AudioStream* astrm = NULL;
  AudioFileDecoder* dec = NULL;
  try{
    dec = AudioFileDecoder::getDecoder();
  }catch(Exception){
    delete dec;
    result.errorMessage = "Could not get decoder.";
    return result;
  }

  try{
    astrm = dec->decodeFile(object.filePath);
    delete dec;
  }catch(Exception){
    delete astrm;
    delete dec;
    result.errorMessage = "Could not decode file.";
    return result;
  }

  // make audio stream monaural
  astrm->reduceToMono();

  // downsample if necessary
  if(object.prefs.getDFactor() > 1){
    Downsampler* ds = Downsampler::getDownsampler(object.prefs.getDFactor(),astrm->getFrameRate(),object.prefs.getLastFreq());
    try{
      astrm = ds->downsample(astrm,object.prefs.getDFactor());
    }catch(Exception){
      delete astrm;
      delete ds;
      result.errorMessage = "Downsampler failed.";
      return result;
    }
    delete ds;
  }

  // start spectrum analysis
  SpectrumAnalyser* sa = NULL;
  Chromagram* ch = NULL;
  sa = SpectrumAnalyserFactory::getInstance()->getSpectrumAnalyser(astrm->getFrameRate(),object.prefs);
  ch = sa->chromagram(astrm);
  delete astrm; // note we don't delete the spectrum analyser; it stays in the centralised factory for reuse.
  ch->reduceTuningBins(object.prefs);
  if(object.batchRow == -1)
    result.fullChromagram = Chromagram(*ch);

  // reduce chromagram
  ch->reduceToOneOctave(object.prefs);
  if(object.batchRow == -1)
    result.oneOctaveChromagram = Chromagram(*ch);

  // get energy level across track to weight segments
  std::vector<float> loudness(ch->getHops());
  for(int h=0; h<ch->getHops(); h++)
    for(int b=0; b<ch->getBins(); b++)
      loudness[h] += ch->getMagnitude(h,b);

  // get harmonic change signal
  Segmentation* hcdf = Segmentation::getSegmentation(object.prefs);
  result.harmonicChangeSignal = hcdf->getRateOfChange(ch,object.prefs);

  // get track segmentation
  std::vector<int> changes = hcdf->getSegments(result.harmonicChangeSignal,object.prefs);
  delete hcdf;
  changes.push_back(ch->getHops()); // It used to be getHops()-1. But this doesn't crash. So we like it.

  // get key estimates for segments
  KeyClassifier hc(object.prefs);
  result.keyEstimates.clear();
  std::vector<float> keyWeights(24);
  for(int i=0; i<(signed)changes.size()-1; i++){
    std::vector<double> chroma(ch->getBins());
    for(int j=changes[i]; j<changes[i+1]; j++)
      for(int k=0; k<ch->getBins(); k++)
        chroma[k] += ch->getMagnitude(j,k);
    int key = hc.classify(chroma);
    for(int j=changes[i]; j<changes[i+1]; j++){
      result.keyEstimates.push_back(key);
      if(key < 24) // ignore parts that were classified as silent
        keyWeights[key] += loudness[j];
    }
  }
  result.keyEstimates.push_back(result.keyEstimates[result.keyEstimates.size()-1]); // put last key on again to match length of track

  // get global key
  result.globalKeyEstimate = 24;
  float mostCommonKeyWeight = 0.0;
  for(int i=0; i<(signed)keyWeights.size(); i++){
    if(keyWeights[i] > mostCommonKeyWeight){
      mostCommonKeyWeight = keyWeights[i];
      result.globalKeyEstimate = i;
    }
  }

  return result;
}
