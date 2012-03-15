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

#include "preferences.h"

Preferences::Preferences(){

  QSettings settings;

  // ======================= Musical frequencies ===============================

  // Bear in mind that KeyFinder params object has built in defaults

  settings.beginGroup("analysisFrequencies");
  if(settings.contains("startingFrequencyA")){
    core.setStartingFreqA(settings.value("startingFrequencyA").toFloat());
  }
  if(settings.contains("numOctaves")){
    core.setOctaves((unsigned)settings.value("numOctaves").toInt());
  }
  if(settings.contains("offsetToC")){
    core.setOffsetToC(settings.value("offsetToC").toBool());
  }
  if(settings.contains("bandsPerSemitone")){
    core.setBps((unsigned)settings.value("bandsPerSemitone").toInt());
  }
  settings.endGroup();

  // ======================== Spectral analysis ================================

  settings.beginGroup("spectralAnalysis");
  if(settings.contains("temporalWindow")){
    QVariant val = settings.value("temporalWindow");
    if(strcmp(val.typeName(),"QChar") == 0){
      // backward compatibility; used to be stored as char before libKeyFinder
      // TODO
    }else{
      core.setTemporalWindow((KeyFinder::temporal_window_t)val.toInt());
    }
  }
  if(settings.contains("fftFrameSize")){
    core.setFftFrameSize((unsigned)settings.value("fftFrameSize").toInt());
  }
  if(settings.contains("hopSize")){
    core.setHopSize((unsigned)settings.value("hopSize").toInt());
  }
  if(settings.contains("directSkStretch")){
    core.setDirectSkStretch(settings.value("directSkStretch").toFloat());
  }
  settings.endGroup();

  // ========================== Downsampling ===================================

  settings.beginGroup("downsampling");
  if(settings.contains("dFactor")){
    dFactor = settings.value("dFactor").toInt();
  }else{
    int defaultVal = 10;
    settings.setValue("dFactor",defaultVal);
    dFactor = defaultVal;
    qDebug("Wrote default dFactor (%d)",dFactor);
  }
  settings.endGroup();

  // ============================= Tuning ======================================

  settings.beginGroup("tuning");
  if(settings.contains("tuningMethod")){
    QVariant val = settings.value("tuningMethod");
    if(strcmp(val.typeName(),"TODO") == 0){
      // backward compatibility; used to be stored as int before libKeyFinder
      // TODO, but may not be necessary if enum in same order
    }else{
      core.setTuningMethod((KeyFinder::tuning_method_t)val.toInt());
    }
  }
  if(settings.contains("detunedBandWeight")){
    core.setDetunedBandWeight(settings.value("detunedBandWeight").toFloat());
  }
  settings.endGroup();

  // ================ Harmonic Change Detection Function =======================

  settings.beginGroup("harmonicChangeDetectionFunction");
  if(settings.contains("hcdf")){
    QVariant val = settings.value("hcdf");
    if(strcmp(val.typeName(),"QChar") == 0){
      // backward compatibility; used to be stored as char before libKeyFinder
      // TODO
    }else{
      core.setSegmentation((KeyFinder::segmentation_t)val.toInt());
    }
  }
  if(settings.contains("hcdfGaussianSize")){
    core.setHcdfGaussianSize((unsigned)settings.value("hcdfGaussianSize").toInt());
  }
  if(settings.contains("hcdfGaussianSigma")){
    core.setHcdfGaussianSigma(settings.value("hcdfGaussianSigma").toFloat());
  }
  if(settings.contains("hcdfPeakPickingNeighbours")){
    core.setHcdfPeakPickingNeighbours((unsigned)settings.value("hcdfPeakPickingNeighbours").toInt());
  }
  if(settings.contains("hcdfArbitrarySegments")){
    core.setArbitrarySegments((unsigned)settings.value("hcdfArbitrarySegments").toInt());
  }
  settings.endGroup();

  // ========================= Key classification ==============================

  settings.beginGroup("keyClassification");
  if(settings.contains("toneProfile")){
    QVariant val = settings.value("toneProfile");
    if(strcmp(val.typeName(),"TODO") == 0){
      // backward compatibility; used to be stored as int before libKeyFinder
      // TODO, but may not be necessary if enum in same order
    }else{
      core.setToneProfile((KeyFinder::tone_profile_t)val.toInt());
    }
  }
  if(settings.contains("similarityMeasure")){
    QVariant val = settings.value("similarityMeasure");
    if(strcmp(val.typeName(),"QChar") == 0){
      // backward compatibility; used to be stored as char before libKeyFinder
      // TODO
    }else{
      core.setSimilarityMeasure((KeyFinder::similarity_measure_t)val.toInt());
    }
  }
  settings.endGroup();

  // ========================= Custom Tone Profile ==============================

  settings.beginGroup("customToneProfile");
  if(settings.contains("maj0")){
    std::vector<float> ctp;
    ctp.push_back(settings.value("maj0").toFloat());
    ctp.push_back(settings.value("maj1").toFloat());
    ctp.push_back(settings.value("maj2").toFloat());
    ctp.push_back(settings.value("maj3").toFloat());
    ctp.push_back(settings.value("maj4").toFloat());
    ctp.push_back(settings.value("maj5").toFloat());
    ctp.push_back(settings.value("maj6").toFloat());
    ctp.push_back(settings.value("maj7").toFloat());
    ctp.push_back(settings.value("maj8").toFloat());
    ctp.push_back(settings.value("maj9").toFloat());
    ctp.push_back(settings.value("maj10").toFloat());
    ctp.push_back(settings.value("maj11").toFloat());
    ctp.push_back(settings.value("min0").toFloat());
    ctp.push_back(settings.value("min1").toFloat());
    ctp.push_back(settings.value("min2").toFloat());
    ctp.push_back(settings.value("min3").toFloat());
    ctp.push_back(settings.value("min4").toFloat());
    ctp.push_back(settings.value("min5").toFloat());
    ctp.push_back(settings.value("min6").toFloat());
    ctp.push_back(settings.value("min7").toFloat());
    ctp.push_back(settings.value("min8").toFloat());
    ctp.push_back(settings.value("min9").toFloat());
    ctp.push_back(settings.value("min10").toFloat());
    ctp.push_back(settings.value("min11").toFloat());
    core.setCustomToneProfile(ctp);
  }
  settings.endGroup();

  // ========================== Custom Key Codes ===============================

  settings.beginGroup("customKeyCodes");
  if(settings.contains("majKey0")){
    customKeyCodes = QStringList();
    // interleaved major/minor to match tone profiling system
    customKeyCodes.push_back(settings.value("majKey0").toString());
    customKeyCodes.push_back(settings.value("minKey0").toString());
    customKeyCodes.push_back(settings.value("majKey1").toString());
    customKeyCodes.push_back(settings.value("minKey1").toString());
    customKeyCodes.push_back(settings.value("majKey2").toString());
    customKeyCodes.push_back(settings.value("minKey2").toString());
    customKeyCodes.push_back(settings.value("majKey3").toString());
    customKeyCodes.push_back(settings.value("minKey3").toString());
    customKeyCodes.push_back(settings.value("majKey4").toString());
    customKeyCodes.push_back(settings.value("minKey4").toString());
    customKeyCodes.push_back(settings.value("majKey5").toString());
    customKeyCodes.push_back(settings.value("minKey5").toString());
    customKeyCodes.push_back(settings.value("majKey6").toString());
    customKeyCodes.push_back(settings.value("minKey6").toString());
    customKeyCodes.push_back(settings.value("majKey7").toString());
    customKeyCodes.push_back(settings.value("minKey7").toString());
    customKeyCodes.push_back(settings.value("majKey8").toString());
    customKeyCodes.push_back(settings.value("minKey8").toString());
    customKeyCodes.push_back(settings.value("majKey9").toString());
    customKeyCodes.push_back(settings.value("minKey9").toString());
    customKeyCodes.push_back(settings.value("majKey10").toString());
    customKeyCodes.push_back(settings.value("minKey10").toString());
    customKeyCodes.push_back(settings.value("majKey11").toString());
    customKeyCodes.push_back(settings.value("minKey11").toString());
    customKeyCodes.push_back(settings.value("silence").toString());
  }else{
    customKeyCodes = QStringList();
    for(int i=0; i<25; i++)
      customKeyCodes << "";
    settings.setValue("majKey0",customKeyCodes[0]);
    settings.setValue("minKey0",customKeyCodes[1]);
    settings.setValue("majKey1",customKeyCodes[2]);
    settings.setValue("minKey1",customKeyCodes[3]);
    settings.setValue("majKey2",customKeyCodes[4]);
    settings.setValue("minKey2",customKeyCodes[5]);
    settings.setValue("majKey3",customKeyCodes[6]);
    settings.setValue("minKey3",customKeyCodes[7]);
    settings.setValue("majKey4",customKeyCodes[8]);
    settings.setValue("minKey4",customKeyCodes[9]);
    settings.setValue("majKey5",customKeyCodes[10]);
    settings.setValue("minKey5",customKeyCodes[11]);
    settings.setValue("majKey6",customKeyCodes[12]);
    settings.setValue("minKey6",customKeyCodes[13]);
    settings.setValue("majKey7",customKeyCodes[14]);
    settings.setValue("minKey7",customKeyCodes[15]);
    settings.setValue("majKey8",customKeyCodes[16]);
    settings.setValue("minKey8",customKeyCodes[17]);
    settings.setValue("majKey9",customKeyCodes[18]);
    settings.setValue("minKey9",customKeyCodes[19]);
    settings.setValue("majKey10",customKeyCodes[20]);
    settings.setValue("minKey10",customKeyCodes[21]);
    settings.setValue("majKey11",customKeyCodes[22]);
    settings.setValue("minKey11",customKeyCodes[23]);
    settings.setValue("silence",customKeyCodes[24]);
    qDebug("Wrote default customKeyCodes (blank)");
  }
  settings.endGroup();

  // ========================= Key classification ==============================

  settings.beginGroup("tags");
  if(settings.contains("tagFormat")){
    QVariant val = settings.value("tagFormat");
    if(strcmp(val.typeName(),"QChar") == 0){
      // backward compatibility; used to be stored as char
      // TODO
    }else{
      tagFormat = (tag_format_t)val.toInt();
    }
  }else{
    tag_format_t defaultVal = TAG_FORMAT_KEYS;
    settings.setValue("tagFormat",defaultVal);
    tagFormat = defaultVal;
    qDebug("Wrote default tagFormat (Keys)");
  }
  if(settings.contains("writeToTagComment")){
    writeToTagComment = settings.value("writeToTagComment").toBool();
    writeToTagGrouping = settings.value("writeToTagGrouping").toBool();
    writeToTagKey = settings.value("writeToTagKey").toBool();
  }else{
    // init
    writeToTagComment = false;
    writeToTagGrouping = false;
    writeToTagKey = false;
    settings.setValue("writeToTagComment",false);
    settings.setValue("writeToTagGrouping",false);
    settings.setValue("writeToTagKey",false);
    // check backwards compatible data
    char oldTagField;
    if(settings.contains("tagField")){
      oldTagField = settings.value("tagField").toChar().toAscii();
      if(oldTagField == 'g'){
        writeToTagGrouping = true;
        settings.setValue("writeToTagGrouping",true);
      }else if(oldTagField == 'k'){
        writeToTagKey = true;
        settings.setValue("writeToTagKey",true);
      }else{
        writeToTagComment = true;
        settings.setValue("writeToTagComment",true);
      }
      qDebug("Wrote writeToTags from old tagField setting");
    }else{
      writeToTagComment = true;
      settings.setValue("writeToTagComment",true);
      qDebug("Wrote default writeToTags (Comment)");
    }
  }
  if(settings.contains("writeTagsAutomatically")){
    writeTagsAutomatically = settings.value("writeTagsAutomatically").toBool();
  }else{
    bool defaultVal = false;
    settings.setValue("writeTagsAutomatically",defaultVal);
    writeTagsAutomatically = defaultVal;
    qDebug("Wrote default writeTagsAutomatically (false)");
  }
  settings.endGroup();

  // =========================== Batch jobs ================================

  settings.beginGroup("batch");
  if(settings.contains("parallelBatchJobs")){
    parallelBatchJobs = settings.value("parallelBatchJobs").toBool();
  }else{
    bool defaultVal = true;
    settings.setValue("parallelBatchJobs",defaultVal);
    parallelBatchJobs = defaultVal;
    qDebug("Wrote default parallelBatchJobs (true)");
  }
  if(settings.contains("skipFilesWithExistingTags")){
    skipFilesWithExistingTags = settings.value("skipFilesWithExistingTags").toBool();
  }else{
    bool defaultVal = false;
    settings.setValue("skipFilesWithExistingTags",defaultVal);
    skipFilesWithExistingTags = defaultVal;
    qDebug("Wrote default skipFilesWithExistingTags (false)");
  }
  settings.endGroup();

  // ============================= Library ==================================

  settings.beginGroup("library");
  if(settings.contains("iTunesLibraryPath")){
    iTunesLibraryPath = settings.value("iTunesLibraryPath").toString();
  }else{
    QString defaultVal;
#ifdef Q_OS_WIN
    defaultVal = QDir::homePath() + "/My Music/iTunes/iTunes Music Library.xml";
#else
    defaultVal = QDir::homePath() + "/Music/iTunes/iTunes Music Library.xml";
#endif
    settings.setValue("iTunesLibraryPath",defaultVal);
    iTunesLibraryPath = defaultVal;
    qDebug("Wrote default iTunesLibraryPath");
  }
  if(settings.contains("traktorLibraryPath")){
    traktorLibraryPath = settings.value("traktorLibraryPath").toString();
  }else{
    QString defaultVal;
#ifdef Q_OS_WIN
    defaultVal = QDir::homePath() + "/My Documents/Native Instruments/Traktor 2.1.2/collection.nml";
#else
    defaultVal = QDir::homePath() + "/Documents/Native Instruments/Traktor 2.1.2/collection.nml";
#endif
    settings.setValue("traktorLibraryPath",defaultVal);
    traktorLibraryPath = defaultVal;
    qDebug("Wrote default traktorLibraryPath");
  }
  if(settings.contains("seratoLibraryPath")){
    seratoLibraryPath = settings.value("seratoLibraryPath").toString();
  }else{
    QString defaultVal;
#ifdef Q_OS_WIN
    defaultVal = QDir::homePath() + "/My Music/_Serato_/database V2";
#else
    defaultVal = QDir::homePath() + "/Music/_Serato_/database V2";
#endif
    settings.setValue("seratoLibraryPath",defaultVal);
    seratoLibraryPath = defaultVal;
    qDebug("Wrote default seratoLibraryPath");
  }
  settings.endGroup();

  // ============================= UIState ==================================

  settings.beginGroup("uistate");
  if(settings.contains("batchWindowState")){
    batchWindowState = settings.value("batchWindowState").toByteArray();
  }else{
    batchWindowState = QByteArray();
  }
  if(settings.contains("batchWindowGeometry")){
    batchWindowGeometry = settings.value("batchWindowGeometry").toByteArray();
  }else{
    batchWindowGeometry = QByteArray();
  }
  if(settings.contains("batchWindowSplitterState")){
    batchWindowSplitterState = settings.value("batchWindowSplitterState").toByteArray();
  }else{
    batchWindowSplitterState = QByteArray();
  }
  settings.endGroup();

  // ================================ End ======================================

  // default key codes
  defaultKeyCodes.push_back("A");		defaultKeyCodes.push_back("Am");
  defaultKeyCodes.push_back("Bb");	defaultKeyCodes.push_back("Bbm");
  defaultKeyCodes.push_back("B");		defaultKeyCodes.push_back("Bm");
  defaultKeyCodes.push_back("C");		defaultKeyCodes.push_back("Cm");
  defaultKeyCodes.push_back("Db");	defaultKeyCodes.push_back("Dbm");
  defaultKeyCodes.push_back("D");		defaultKeyCodes.push_back("Dm");
  defaultKeyCodes.push_back("Eb");	defaultKeyCodes.push_back("Ebm");
  defaultKeyCodes.push_back("E");		defaultKeyCodes.push_back("Em");
  defaultKeyCodes.push_back("F");		defaultKeyCodes.push_back("Fm");
  defaultKeyCodes.push_back("Gb");	defaultKeyCodes.push_back("Gbm");
  defaultKeyCodes.push_back("G");		defaultKeyCodes.push_back("Gm");
  defaultKeyCodes.push_back("Ab");	defaultKeyCodes.push_back("Abm");
  defaultKeyCodes.push_back(".");
  // key colours for display
  keyColours.push_back(qRgb(223,255,127));	keyColours.push_back(qRgb(191,223,127));
  keyColours.push_back(qRgb(255,127,255));	keyColours.push_back(qRgb(223,127,223));
  keyColours.push_back(qRgb(127,255,223));	keyColours.push_back(qRgb(127,223,191));
  keyColours.push_back(qRgb(255,127,127));	keyColours.push_back(qRgb(223,127,127));
  keyColours.push_back(qRgb(127,223,255));	keyColours.push_back(qRgb(127,191,223));
  keyColours.push_back(qRgb(255,255,127));	keyColours.push_back(qRgb(223,223,127));
  keyColours.push_back(qRgb(223,127,255));	keyColours.push_back(qRgb(191,127,223));
  keyColours.push_back(qRgb(127,255,127));	keyColours.push_back(qRgb(127,223,127));
  keyColours.push_back(qRgb(255,127,223));	keyColours.push_back(qRgb(223,127,191));
  keyColours.push_back(qRgb(127,255,255));	keyColours.push_back(qRgb(127,223,223));
  keyColours.push_back(qRgb(255,223,127));	keyColours.push_back(qRgb(223,191,127));
  keyColours.push_back(qRgb(127,127,255));	keyColours.push_back(qRgb(127,127,223));
  keyColours.push_back(qRgb(127,127,127));
}

void Preferences::save(){
  QSettings settings;

  settings.beginGroup("analysisFrequencies");
  settings.setValue("startingFrequencyA", core.getStartingFreqA());
  settings.setValue("numOctaves", core.getOctaves());
  settings.setValue("offsetToC", core.getOffsetToC());
  settings.setValue("bandsPerSemitone", core.getBpo() / 12);
  settings.endGroup();

  settings.beginGroup("spectralAnalysis");
  settings.setValue("temporalWindow", core.getTemporalWindow());
  settings.setValue("fftFrameSize", core.getFftFrameSize());
  settings.setValue("hopSize", core.getHopSize());
  settings.setValue("directSkStretch", core.getDirectSkStretch());
  settings.endGroup();

  settings.beginGroup("downsampling");
  settings.setValue("dFactor", dFactor);
  settings.endGroup();

  settings.beginGroup("tuning");
  settings.setValue("tuningMethod", core.getTuningMethod());
  settings.setValue("detunedBandWeight", core.getDetunedBandWeight());
  settings.endGroup();

  settings.beginGroup("harmonicChangeDetectionFunction");
  settings.setValue("hcdf", core.getSegmentation());
  settings.setValue("hcdfGaussianSize", core.getHcdfGaussianSize());
  settings.setValue("hcdfGaussianSigma", core.getHcdfGaussianSigma());
  settings.setValue("hcdfPeakPickingNeighbours", core.getHcdfPeakPickingNeighbours());
  settings.setValue("hcdfArbitrarySegments", core.getArbitrarySegments());
  settings.endGroup();

  settings.beginGroup("keyClassification");
  settings.setValue("toneProfile", core.getToneProfile());
  settings.setValue("similarityMeasure", core.getSimilarityMeasure());
  settings.endGroup();

  settings.beginGroup("customToneProfile");
  std::vector<float> ctp = core.getCustomToneProfile();
  settings.setValue("maj0", ctp[0]);
  settings.setValue("maj1", ctp[1]);
  settings.setValue("maj2", ctp[2]);
  settings.setValue("maj3", ctp[3]);
  settings.setValue("maj4", ctp[4]);
  settings.setValue("maj5", ctp[5]);
  settings.setValue("maj6", ctp[6]);
  settings.setValue("maj7", ctp[7]);
  settings.setValue("maj8", ctp[8]);
  settings.setValue("maj9", ctp[9]);
  settings.setValue("maj10", ctp[10]);
  settings.setValue("maj11", ctp[11]);
  settings.setValue("min0", ctp[12]);
  settings.setValue("min1", ctp[13]);
  settings.setValue("min2", ctp[14]);
  settings.setValue("min3", ctp[15]);
  settings.setValue("min4", ctp[16]);
  settings.setValue("min5", ctp[17]);
  settings.setValue("min6", ctp[18]);
  settings.setValue("min7", ctp[19]);
  settings.setValue("min8", ctp[20]);
  settings.setValue("min9", ctp[21]);
  settings.setValue("min10", ctp[22]);
  settings.setValue("min11", ctp[23]);
  settings.endGroup();

  settings.beginGroup("customKeyCodes");
  settings.setValue("majKey0", customKeyCodes[0]);
  settings.setValue("majKey1", customKeyCodes[1]);
  settings.setValue("majKey2", customKeyCodes[2]);
  settings.setValue("majKey3", customKeyCodes[3]);
  settings.setValue("majKey4", customKeyCodes[4]);
  settings.setValue("majKey5", customKeyCodes[5]);
  settings.setValue("majKey6", customKeyCodes[6]);
  settings.setValue("majKey7", customKeyCodes[7]);
  settings.setValue("majKey8", customKeyCodes[8]);
  settings.setValue("majKey9", customKeyCodes[9]);
  settings.setValue("majKey10", customKeyCodes[10]);
  settings.setValue("majKey11", customKeyCodes[11]);
  settings.setValue("minKey0", customKeyCodes[12]);
  settings.setValue("minKey1", customKeyCodes[13]);
  settings.setValue("minKey2", customKeyCodes[14]);
  settings.setValue("minKey3", customKeyCodes[15]);
  settings.setValue("minKey4", customKeyCodes[16]);
  settings.setValue("minKey5", customKeyCodes[17]);
  settings.setValue("minKey6", customKeyCodes[18]);
  settings.setValue("minKey7", customKeyCodes[19]);
  settings.setValue("minKey8", customKeyCodes[20]);
  settings.setValue("minKey9", customKeyCodes[21]);
  settings.setValue("minKey10", customKeyCodes[22]);
  settings.setValue("minKey11", customKeyCodes[23]);
  settings.setValue("silence", customKeyCodes[24]);
  settings.endGroup();

  settings.beginGroup("tags");
  settings.setValue("tagFormat", tagFormat);
  settings.setValue("writeToTagComment", writeToTagComment);
  settings.setValue("writeToTagGrouping", writeToTagGrouping);
  settings.setValue("writeToTagKey", writeToTagKey);
  settings.setValue("writeTagsAutomatically", writeTagsAutomatically);
  settings.endGroup();

  settings.beginGroup("batch");
  settings.setValue("parallelBatchJobs", parallelBatchJobs);
  settings.setValue("skipFilesWithExistingTags", skipFilesWithExistingTags);
  settings.endGroup();

  settings.beginGroup("library");
  settings.setValue("iTunesLibraryPath", iTunesLibraryPath);
  settings.setValue("traktorLibraryPath", traktorLibraryPath);
  settings.setValue("seratoLibraryPath", seratoLibraryPath);
  settings.endGroup();

  settings.beginGroup("uistate");
  settings.setValue("batchWindowState", batchWindowState);
  settings.setValue("batchWindowGeometry", batchWindowGeometry);
  settings.setValue("batchWindowSplitterState", batchWindowSplitterState);
  settings.endGroup();

}

Preferences& Preferences::operator=(const Preferences& that){
  if(this != &that){
    core = that.core;
    dFactor = that.dFactor;
    customKeyCodes = that.customKeyCodes;
    tagFormat = that.tagFormat;
    writeToTagComment = that.writeToTagComment;
    writeToTagGrouping = that.writeToTagGrouping;
    writeToTagKey = that.writeToTagKey;
    writeTagsAutomatically = that.writeTagsAutomatically;
    skipFilesWithExistingTags = that.skipFilesWithExistingTags;
    parallelBatchJobs = that.parallelBatchJobs;
    iTunesLibraryPath = that.iTunesLibraryPath;
    traktorLibraryPath = that.traktorLibraryPath;
    seratoLibraryPath = that.seratoLibraryPath;
    batchWindowState = that.batchWindowState;
    batchWindowGeometry = that.batchWindowGeometry;
    batchWindowSplitterState = that.batchWindowSplitterState;
  }
  return *this;
}


bool Preferences::equivalentSpectralAnalysis(const Preferences& that) const{
  if(core.getTemporalWindow() != that.core.getTemporalWindow())
    return false;
  if(core.getBpo() != that.core.getBpo())
    return false;
  if(core.getStartingFreqA() != that.core.getStartingFreqA())
    return false;
  if(core.getOctaves() != that.core.getOctaves())
    return false;
  if(core.getOffsetToC() != that.core.getOffsetToC())
    return false;
  if(core.getFftFrameSize() != that.core.getFftFrameSize())
    return false;
  if(core.getDirectSkStretch() != that.core.getDirectSkStretch())
    return false;
  return true;
}

bool         Preferences::getWriteTagsAutomatically()    const { return writeTagsAutomatically; }
bool         Preferences::getParallelBatchJobs()         const { return parallelBatchJobs; }
bool         Preferences::getWriteToTagComment()         const { return writeToTagComment; }
bool         Preferences::getWriteToTagGrouping()        const { return writeToTagGrouping; }
bool         Preferences::getWriteToTagKey()             const { return writeToTagKey; }
bool         Preferences::getSkipFilesWithExistingTags() const { return skipFilesWithExistingTags; }
bool         Preferences::getOffsetToC()                 const { return core.getOffsetToC(); }
char         Preferences::getTemporalWindow()            const { return core.getTemporalWindow(); }
char         Preferences::getHcdf()                      const { return core.getSegmentation(); }
char         Preferences::getSimilarityMeasure()         const { return core.getSimilarityMeasure(); }
char         Preferences::getTagFormat()                 const { return tagFormat; }
int          Preferences::getHopSize()                   const { return core.getHopSize(); }
int          Preferences::getFftFrameSize()              const { return core.getFftFrameSize(); }
int          Preferences::getOctaves()                   const { return core.getOctaves(); }
int          Preferences::getBpo()                       const { return core.getBpo(); }
int          Preferences::getDFactor()                   const { return dFactor; }
int          Preferences::getToneProfile()               const { return core.getToneProfile(); }
int          Preferences::getTuningMethod()              const { return core.getTuningMethod(); }
int          Preferences::getHcdfArbitrarySegments()     const { return core.getArbitrarySegments(); }
int          Preferences::getHcdfPeakPickingNeighbours() const { return core.getHcdfPeakPickingNeighbours(); }
int          Preferences::getHcdfGaussianSize()          const { return core.getHcdfGaussianSize(); }
float        Preferences::getHcdfGaussianSigma()         const { return core.getHcdfGaussianSigma(); }
float        Preferences::getStartingFreqA()             const { return core.getStartingFreqA(); }
float        Preferences::getDirectSkStretch()           const { return core.getDirectSkStretch(); }
float        Preferences::getDetunedBandWeight()         const { return core.getDetunedBandWeight(); }
QString      Preferences::getITunesLibraryPath()         const { return iTunesLibraryPath; }
QString      Preferences::getTraktorLibraryPath()        const { return traktorLibraryPath; }
QString      Preferences::getSeratoLibraryPath()         const { return seratoLibraryPath; }
QList<float> Preferences::getCustomToneProfile()         const { return core.getCustomToneProfile(); }
QStringList  Preferences::getCustomKeyCodes()            const { return customKeyCodes; }
QByteArray   Preferences::getBatchWindowState()          const { return batchWindowState; }
QByteArray   Preferences::getBatchWindowGeometry()       const { return batchWindowGeometry; }
QByteArray   Preferences::getBatchWindowSplitterState()  const { return batchWindowSplitterState; }

void Preferences::setWriteTagsAutomatically(bool autoTags)          { writeTagsAutomatically = autoTags; }
void Preferences::setWriteToTagComment(bool cmt)                    { writeToTagComment = cmt; }
void Preferences::setWriteToTagGrouping(bool grp)                   { writeToTagGrouping = grp; }
void Preferences::setWriteToTagKey(bool key)                        { writeToTagKey = key; }
void Preferences::setSkipFilesWithExistingTags(bool skip)           { skipFilesWithExistingTags = skip; }
void Preferences::setOffsetToC(bool off)                            { core.setOffsetToC(off); }
void Preferences::setTemporalWindow(char window)                    { core.setTemporalWindow(window); }
void Preferences::setHcdf(char h)                                   { core.setSegmentation(h); }
void Preferences::setSimilarityMeasure(char msr)                    { core.setSimilarityMeasure(msr); }
void Preferences::setTagFormat(char fmt)                            { tagFormat = fmt; }
void Preferences::setHopSize(int size)                              { core.setHopSize(size); }
void Preferences::setFftFrameSize(int framesize)                    { core.setFftFrameSize(framesize); }
void Preferences::setOctaves(int oct)                               { core.setOctaves(oct); }
void Preferences::setBps(int bands)                                 { core.setBps(bands); }
void Preferences::setDFactor(int factor)                            { dFactor = factor; }
void Preferences::setToneProfile(int profile)                       { core.setToneProfile(profile); }
void Preferences::setHcdfArbitrarySegments(int segments)            { core.setArbitrarySegments(segments); }
void Preferences::setHcdfPeakPickingNeighbours(int neighbours)      { core.setHcdfPeakPickingNeighbours(neighbours); }
void Preferences::setHcdfGaussianSize(int size)                     { core.setHcdfGaussianSize(size); }
void Preferences::setTuningMethod(int tune)                         { core.setTuningMethod(tune); }
void Preferences::setHcdfGaussianSigma(float sigma)                 { core.setHcdfGaussianSigma(sigma); }
void Preferences::setStartingFreqA(float a)                         { core.setStartingFreqA(a); }
void Preferences::setDirectSkStretch(float stretch)                 { core.setDirectSkStretch(stretch); }
void Preferences::setDetunedBandWeight(float weight)                { core.setDetunedBandWeight(weight); }
void Preferences::setITunesLibraryPath(const QString& path)         { iTunesLibraryPath = path; }
void Preferences::setTraktorLibraryPath(const QString& path)        { traktorLibraryPath = path; }
void Preferences::setSeratoLibraryPath(const QString& path)         { seratoLibraryPath = path; }
void Preferences::setCustomToneProfile(const QList<float>& profile) { core.setCustomToneProfile(profile); }
void Preferences::setCustomKeyCodes(const QStringList& codes)       { customKeyCodes = codes; }
void Preferences::setBatchWindowState(const QByteArray& a)          { batchWindowState = a; }
void Preferences::setBatchWindowGeometry(const QByteArray& a)       { batchWindowGeometry = a; }
void Preferences::setBatchWindowSplitterState(const QByteArray& a)  { batchWindowSplitterState = a; }

void Preferences::setParallelBatchJobs(bool parallel){
  parallelBatchJobs = parallel;
  int numThreads = QThread::idealThreadCount();
  if(numThreads == -1 || parallel == false){
    QThreadPool::globalInstance()->setMaxThreadCount(1);
  }else{
    QThreadPool::globalInstance()->setMaxThreadCount(numThreads);
  }
}

float Preferences::getBinFreq(int n)const{
  if(n >= octaves*12*bps){
    qDebug("Attempt to get out-of-bounds frequency index (%d/%d)",n,octaves*12*bps);
    return 0;
  }
  return binFreqs[n];
}

float Preferences::getLastFreq() const{
  return binFreqs[binFreqs.size()-1];
}

QString Preferences::getKeyCode(int n) const{
  if(n < 0 || n >= (signed)defaultKeyCodes.size()){
    qDebug("Attempt to get name of out-of-bounds key (%d/%d)",n,(int)defaultKeyCodes.size());
    return "";
  }
  QString defaultCode = defaultKeyCodes[n];
  QString customCode = customKeyCodes[n];
  if(customCode == "" || tagFormat == 'k')
    return defaultCode;
  else if(tagFormat == 'c')
    return customCode;
  else
    return customCode + " " + defaultCode;
}

QColor Preferences::getKeyColour(int n) const{
  if(n < 0 || n >= (signed)keyColours.size()){
    qDebug("Attempt to get colour of out-of-bounds key (%d/%d)",n,(int)keyColours.size());
    return qRgb(0,0,0);
  }
  return keyColours[n];
}

void Preferences::generateBinFreqs(){
  int bpo = bps * 12;
  binFreqs = QList<float>();
  float freqRatio = pow(2, 1.0 / bpo);
  float octFreq = stFreq;
  float binFreq;
  int concertPitchBin = bps/2;
  for(int i = 0; i < octaves; i++){
    binFreq = octFreq;
    // offset as required
    if(offsetToC){
      binFreq *= pow(freqRatio, 3);
    }
    // tune down for bins before first concert pitch bin (if bps > 1)
    for(int j = 0; j < concertPitchBin; j++)
      binFreqs << binFreq / pow(freqRatio, concertPitchBin - j);
    // and tune all other bins
    for(int j = concertPitchBin; j < bpo; j++){
      binFreqs << binFreq;
      binFreq *= freqRatio;
    }
    octFreq *= 2;
  }
}

void Preferences::setImageColours(QImage& image, int which) const{
  if(which==0){
    // Izo colour scheme
    image.setColor(0,qRgb(0,0,0));
    image.setColor(1,qRgb(0,1,3));
    image.setColor(2,qRgb(1,4,8));
    image.setColor(3,qRgb(1,7,12));
    image.setColor(4,qRgb(2,9,17));
    image.setColor(5,qRgb(2,12,24));
    image.setColor(6,qRgb(3,13,24));
    image.setColor(7,qRgb(4,15,27));
    image.setColor(8,qRgb(5,16,30));
    image.setColor(9,qRgb(6,18,33));
    image.setColor(10,qRgb(7,20,36));
    image.setColor(11,qRgb(8,22,39));
    image.setColor(12,qRgb(9,23,42));
    image.setColor(13,qRgb(10,25,45));
    image.setColor(14,qRgb(11,26,48));
    image.setColor(15,qRgb(14,29,50));
    image.setColor(16,qRgb(18,31,52));
    image.setColor(17,qRgb(21,33,54));
    image.setColor(18,qRgb(24,36,55));
    image.setColor(19,qRgb(27,38,57));
    image.setColor(20,qRgb(32,40,56));
    image.setColor(21,qRgb(38,43,55));
    image.setColor(22,qRgb(43,45,54));
    image.setColor(23,qRgb(49,48,53));
    image.setColor(24,qRgb(56,51,52));
    image.setColor(25,qRgb(65,53,50));
    image.setColor(26,qRgb(74,56,47));
    image.setColor(27,qRgb(84,58,45));
    image.setColor(28,qRgb(93,61,43));
    image.setColor(29,qRgb(103,64,40));
    image.setColor(30,qRgb(113,67,39));
    image.setColor(31,qRgb(123,69,37));
    image.setColor(32,qRgb(133,71,36));
    image.setColor(33,qRgb(142,74,34));
    image.setColor(34,qRgb(152,76,32));
    image.setColor(35,qRgb(160,79,32));
    image.setColor(36,qRgb(170,82,32));
    image.setColor(37,qRgb(179,85,32));
    image.setColor(38,qRgb(188,88,32));
    image.setColor(39,qRgb(196,91,32));
    image.setColor(40,qRgb(203,96,33));
    image.setColor(41,qRgb(210,101,34));
    image.setColor(42,qRgb(216,106,35));
    image.setColor(43,qRgb(223,112,36));
    image.setColor(44,qRgb(230,118,38));
    image.setColor(45,qRgb(236,123,39));
    image.setColor(46,qRgb(239,134,44));
    image.setColor(47,qRgb(241,147,49));
    image.setColor(48,qRgb(242,160,55));
    image.setColor(49,qRgb(244,173,60));
    image.setColor(50,qRgb(246,186,66));
    image.setColor(51,qRgb(248,200,72));
    image.setColor(52,qRgb(249,209,82));
    image.setColor(53,qRgb(249,218,92));
    image.setColor(54,qRgb(249,226,102));
    image.setColor(55,qRgb(250,234,113));
    image.setColor(56,qRgb(250,243,122));
    image.setColor(57,qRgb(251,251,132));
    image.setColor(58,qRgb(251,255,147));
    image.setColor(59,qRgb(252,255,163));
    image.setColor(60,qRgb(253,255,178));
    image.setColor(61,qRgb(253,255,194));
    image.setColor(62,qRgb(254,255,210));
    image.setColor(63,qRgb(254,255,225));
    image.setColor(64,qRgb(255,255,241));
  }else if(which==1){
    // Mono colour scheme
    for(int i=0; i<64; i++){
      image.setColor(i,qRgb(i*4,i*4,i*4));
    }
    image.setColor(64,qRgb(0,255,0));
    image.setColor(64,qRgb(255,255,255));
  }else if(which==2){
    // Pred colour scheme
    image.setColor(0,qRgb(0,0,0));
    image.setColor(1,qRgb(0,0,24));
    image.setColor(2,qRgb(0,0,71));
    image.setColor(3,qRgb(0,0,125));
    image.setColor(4,qRgb(0,0,177));
    image.setColor(5,qRgb(0,0,219));
    image.setColor(6,qRgb(0,0,238));
    image.setColor(7,qRgb(0,0,246));
    image.setColor(8,qRgb(0,0,254));
    image.setColor(9,qRgb(0,13,255));
    image.setColor(10,qRgb(0,27,255));
    image.setColor(11,qRgb(0,45,255));
    image.setColor(12,qRgb(0,69,255));
    image.setColor(13,qRgb(0,88,255));
    image.setColor(14,qRgb(0,103,255));
    image.setColor(15,qRgb(0,117,255));
    image.setColor(16,qRgb(0,139,255));
    image.setColor(17,qRgb(0,154,255));
    image.setColor(18,qRgb(0,165,255));
    image.setColor(19,qRgb(0,178,255));
    image.setColor(20,qRgb(0,200,255));
    image.setColor(21,qRgb(0,220,255));
    image.setColor(22,qRgb(0,236,255));
    image.setColor(23,qRgb(0,250,255));
    image.setColor(24,qRgb(9,255,246));
    image.setColor(25,qRgb(23,255,232));
    image.setColor(26,qRgb(40,255,214));
    image.setColor(27,qRgb(61,255,193));
    image.setColor(28,qRgb(84,255,173));
    image.setColor(29,qRgb(99,255,161));
    image.setColor(30,qRgb(113,255,150));
    image.setColor(31,qRgb(132,255,132));
    image.setColor(32,qRgb(150,255,113));
    image.setColor(33,qRgb(161,255,99));
    image.setColor(34,qRgb(173,255,84));
    image.setColor(35,qRgb(193,255,61));
    image.setColor(36,qRgb(214,255,40));
    image.setColor(37,qRgb(232,255,23));
    image.setColor(38,qRgb(246,255,9));
    image.setColor(39,qRgb(255,250,0));
    image.setColor(40,qRgb(255,236,0));
    image.setColor(41,qRgb(255,220,0));
    image.setColor(42,qRgb(255,200,0));
    image.setColor(43,qRgb(255,178,0));
    image.setColor(44,qRgb(255,165,0));
    image.setColor(45,qRgb(255,154,0));
    image.setColor(46,qRgb(255,139,0));
    image.setColor(47,qRgb(255,117,0));
    image.setColor(48,qRgb(255,103,0));
    image.setColor(49,qRgb(255,88,0));
    image.setColor(50,qRgb(255,69,0));
    image.setColor(51,qRgb(255,45,0));
    image.setColor(52,qRgb(255,27,0));
    image.setColor(53,qRgb(255,13,0));
    image.setColor(54,qRgb(254,0,0));
    image.setColor(55,qRgb(241,0,0));
    image.setColor(56,qRgb(225,0,0));
    image.setColor(57,qRgb(206,0,0));
    image.setColor(58,qRgb(183,0,0));
    image.setColor(59,qRgb(165,0,0));
    image.setColor(60,qRgb(150,0,0));
    image.setColor(61,qRgb(139,0,0));
    image.setColor(62,qRgb(132,0,0));
    image.setColor(63,qRgb(128,0,0));
    image.setColor(64,qRgb(125,0,0));
  }else{
    // Hack colour scheme
    for(int i=0; i<64; i++){
      image.setColor(i,qRgb(0,i*4,0));
    }
    image.setColor(64,qRgb(0,255,0));
  }
}
