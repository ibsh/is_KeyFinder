#include "keyfinderworker.h"

KeyFinderResultSet keyFinderProcessObject(const KeyFinderAnalysisObject& object){

  KeyFinderResultSet resultSet;
  resultSet.batchRow = object.batchRow;

  // initialise stream and decode file into it.
  AudioStream* astrm = NULL;
  AudioFileDecoder* dec = AudioFileDecoder::getDecoder(object.filePath.toLocal8Bit().data());

  try{
    astrm = dec->decodeFile(object.filePath.toLocal8Bit().data());
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
  resultSet.fullChromagram = Chromagram(*ch);

  // reduce chromagram
  ch->reduceToOneOctave(object.prefs);
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
