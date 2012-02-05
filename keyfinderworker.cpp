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

#include "keyfinderworker.h"

KeyFinderResultSet keyFinderProcessObject(const KeyFinderAnalysisObject& object){

  KeyFinderResultSet resultSet;
  resultSet.batchRow = object.batchRow;

  QByteArray encodedPath = QFile::encodeName(object.filePath);
  const char* filePathCh = encodedPath;

  // initialise stream and decode file into it.
  AudioStream* astrm = NULL;
  AudioFileDecoder* dec = NULL;
  try{
    dec = AudioFileDecoder::getDecoder(filePathCh);
  }catch(Exception){
    delete dec;
    resultSet.errorMessage = "Could not get decoder.";
    return resultSet;
  }

  try{
    astrm = dec->decodeFile(filePathCh);
    delete dec;
  }catch(Exception){
    delete astrm;
    delete dec;
    resultSet.errorMessage = "Could not decode file.";
    return resultSet;
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
      resultSet.errorMessage = "Downsampler failed.";
      return resultSet;
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
    resultSet.fullChromagram = Chromagram(*ch);

  // reduce chromagram
  ch->reduceToOneOctave(object.prefs);
  if(object.batchRow == -1)
    resultSet.oneOctaveChromagram = Chromagram(*ch);

  // get energy level across track to weight segments
  std::vector<float> loudness(ch->getHops());
  for(int h=0; h<ch->getHops(); h++)
    for(int b=0; b<ch->getBins(); b++)
      loudness[h] += ch->getMagnitude(h,b);

  // get harmonic change signal
  Segmentation* hcdf = Segmentation::getSegmentation(object.prefs);
  resultSet.harmonicChangeSignal = hcdf->getRateOfChange(ch,object.prefs);

  // get track segmentation
  std::vector<int> changes = hcdf->getSegments(resultSet.harmonicChangeSignal,object.prefs);
  delete hcdf;
  changes.push_back(ch->getHops()); // It used to be getHops()-1. But this doesn't crash. So we like it.

  // get key estimates for segments
  KeyClassifier hc(object.prefs);
  resultSet.keyEstimates.clear();
  std::vector<float> keyWeights(24);
  for(int i=0; i<(signed)changes.size()-1; i++){
    std::vector<double> chroma(ch->getBins());
    for(int j=changes[i]; j<changes[i+1]; j++)
      for(int k=0; k<ch->getBins(); k++)
        chroma[k] += ch->getMagnitude(j,k);
    int key = hc.classify(chroma);
    for(int j=changes[i]; j<changes[i+1]; j++){
      resultSet.keyEstimates.push_back(key);
      if(key < 24) // ignore parts that were classified as silent
        keyWeights[key] += loudness[j];
    }
  }
  resultSet.keyEstimates.push_back(resultSet.keyEstimates[resultSet.keyEstimates.size()-1]); // put last key on again to match length of track

  // get global key
  resultSet.globalKeyEstimate = 24;
  float mostCommonKeyWeight = 0.0;
  for(int i=0; i<(signed)keyWeights.size(); i++){
    if(keyWeights[i] > mostCommonKeyWeight){
      mostCommonKeyWeight = keyWeights[i];
      resultSet.globalKeyEstimate = i;
    }
  }

  return resultSet;
}
