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

#include "preferences.h"

Preferences::Preferences() {

  QSettings settings;

  // ======================= Musical frequencies ===============================

  // Bear in mind that KeyFinder params object has built in defaults

  settings.beginGroup("analysisFrequencies");
  if (settings.contains("startingFrequencyA")) {
    core.setStartingFrequencyA(settings.value("startingFrequencyA").toFloat());
  }
  if (settings.contains("numOctaves")) {
    core.setOctaves((unsigned)settings.value("numOctaves").toInt());
  }
  if (settings.contains("offsetToC")) {
    core.setOffsetToC(settings.value("offsetToC").toBool());
  }
  if (settings.contains("bandsPerSemitone")) {
    core.setBandsPerSemitone((unsigned)settings.value("bandsPerSemitone").toInt());
  }
  settings.endGroup();

  // ======================== Spectral analysis ================================

  settings.beginGroup("spectralAnalysis");
  if (settings.contains("temporalWindow_1_14")) {
    core.setTemporalWindow((KeyFinder::temporal_window_t)settings.value("temporalWindow_1_14").toInt());
  }
  if (settings.contains("fftFrameSize")) {
    core.setFftFrameSize((unsigned)settings.value("fftFrameSize").toInt());
  }
  if (settings.contains("hopsPerFrame")) {
    core.setHopsPerFrame((unsigned)settings.value("hopsPerFrame").toInt());
  }
  if (settings.contains("directSkStretch")) {
    core.setDirectSkStretch(settings.value("directSkStretch").toFloat());
  }
  settings.endGroup();

  // ============================= Tuning ======================================

  settings.beginGroup("tuning");
  if (settings.contains("tuningMethod")) {
    core.setTuningMethod((KeyFinder::tuning_method_t)settings.value("tuningMethod").toInt());
  }
  if (settings.contains("detunedBandWeight")) {
    core.setDetunedBandWeight(settings.value("detunedBandWeight").toFloat());
  }
  settings.endGroup();

  // =========================== Segmentation ==================================

  settings.beginGroup("harmonicChangeDetectionFunction");
  if (settings.contains("segmentation")) {
    core.setSegmentation((KeyFinder::segmentation_t)settings.value("segmentation").toInt());
  }
  if (settings.contains("hcdfGaussianSize")) {
    core.setSegGaussianSize((unsigned)settings.value("hcdfGaussianSize").toInt());
  }
  if (settings.contains("hcdfGaussianSigma")) {
    core.setSegGaussianSigma(settings.value("hcdfGaussianSigma").toFloat());
  }
  if (settings.contains("hcdfPeakPickingNeighbours")) {
    core.setSegPeakPickingNeighbours((unsigned)settings.value("hcdfPeakPickingNeighbours").toInt());
  }
  if (settings.contains("hcdfArbitrarySegments")) {
    core.setArbitrarySegments((unsigned)settings.value("hcdfArbitrarySegments").toInt());
  }
  settings.endGroup();

  // ========================= Key classification ==============================

  settings.beginGroup("keyClassification");
  if (settings.contains("toneProfile_1_14")) {
    core.setToneProfile((KeyFinder::tone_profile_t)settings.value("toneProfile_1_14").toInt());
  }
  if (settings.contains("similarityMeasure_1_14")) {
    core.setSimilarityMeasure((KeyFinder::similarity_measure_t)settings.value("similarityMeasure_1_14").toInt());
  }
  settings.endGroup();

  // ========================= Custom Tone Profile ==============================

  settings.beginGroup("customToneProfile");
  if (settings.contains("maj0")) {
    std::vector<float> ctp;
    // Tone profiles are stored sequentially; major chroma vector then minor chroma vector
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
    try {
      core.setCustomToneProfile(ctp);
    } catch (std::exception& e) {
      qDebug("LibKeyFinder Exception: %s", e.what());
    } catch (...) {
      qDebug("Unknown exception setting custom tone profile.");
    }
  }
  settings.endGroup();

  // ========================== Custom Key Codes ===============================

  settings.beginGroup("customKeyCodes");
  if (settings.contains("SLNC")) {
    customKeyCodes = QStringList();
    customKeyCodes.push_back(settings.value("A").toString());
    customKeyCodes.push_back(settings.value("Am").toString());
    customKeyCodes.push_back(settings.value("Bb").toString());
    customKeyCodes.push_back(settings.value("Bbm").toString());
    customKeyCodes.push_back(settings.value("B").toString());
    customKeyCodes.push_back(settings.value("Bm").toString());
    customKeyCodes.push_back(settings.value("C").toString());
    customKeyCodes.push_back(settings.value("Cm").toString());
    customKeyCodes.push_back(settings.value("Db").toString());
    customKeyCodes.push_back(settings.value("Dbm").toString());
    customKeyCodes.push_back(settings.value("D").toString());
    customKeyCodes.push_back(settings.value("Dm").toString());
    customKeyCodes.push_back(settings.value("Eb").toString());
    customKeyCodes.push_back(settings.value("Ebm").toString());
    customKeyCodes.push_back(settings.value("E").toString());
    customKeyCodes.push_back(settings.value("Em").toString());
    customKeyCodes.push_back(settings.value("F").toString());
    customKeyCodes.push_back(settings.value("Fm").toString());
    customKeyCodes.push_back(settings.value("Gb").toString());
    customKeyCodes.push_back(settings.value("Gbm").toString());
    customKeyCodes.push_back(settings.value("G").toString());
    customKeyCodes.push_back(settings.value("Gm").toString());
    customKeyCodes.push_back(settings.value("Ab").toString());
    customKeyCodes.push_back(settings.value("Abm").toString());
    customKeyCodes.push_back(settings.value("SLNC").toString());
  } else {
    customKeyCodes = QStringList();
    for (int i=0; i<25; i++)
      customKeyCodes << "";
  }
  settings.endGroup();

  // ============================ Metadata =================================

  settings.beginGroup("tags");
  if (settings.contains("metadataFormat")) {
    metadataFormat = (metadata_format_t)settings.value("metadataFormat").toInt();
  } else {
    metadataFormat = METADATA_FORMAT_KEYS;
  }

  if (settings.contains("metadataWriteTitle")) {
    metadataWriteTitle = (metadata_write_t)settings.value("metadataWriteTitle").toInt();
  } else {
    metadataWriteTitle = METADATA_WRITE_NONE;
  }
  if (settings.contains("metadataWriteArtist")) {
    metadataWriteArtist = (metadata_write_t)settings.value("metadataWriteArtist").toInt();
  } else {
    metadataWriteArtist = METADATA_WRITE_NONE;
  }
  if (settings.contains("metadataWriteAlbum")) {
    metadataWriteAlbum = (metadata_write_t)settings.value("metadataWriteAlbum").toInt();
  } else {
    metadataWriteAlbum = METADATA_WRITE_NONE;
  }
  if (settings.contains("metadataWriteComment")) {
    metadataWriteComment = (metadata_write_t)settings.value("metadataWriteComment").toInt();
  } else {
    metadataWriteComment = METADATA_WRITE_PREPEND;
  }
  if (settings.contains("metadataWriteGrouping")) {
    metadataWriteGrouping = (metadata_write_t)settings.value("metadataWriteGrouping").toInt();
  } else {
    metadataWriteGrouping = METADATA_WRITE_NONE;
  }
  if (settings.contains("metadataWriteGrouping")) {
    metadataWriteKey = (metadata_write_t)settings.value("metadataWriteKey").toInt();
  } else {
    metadataWriteKey = METADATA_WRITE_NONE;
  }
  if (settings.contains("metadataWriteFilename")) {
    metadataWriteFilename = (metadata_write_t)settings.value("metadataWriteFilename").toInt();
  } else {
    metadataWriteFilename = METADATA_WRITE_NONE;
  }
  if (settings.contains("metadataDelimiter")) {
    metadataDelimiter = settings.value("metadataDelimiter").toString();
  } else {
    metadataDelimiter = " - ";
  }
  if (settings.contains("writeToFilesAutomatically")) {
    writeToFilesAutomatically = settings.value("writeToFilesAutomatically").toBool();
  } else {
    writeToFilesAutomatically = false;
  }
  settings.endGroup();

  // =========================== Batch jobs ================================

  settings.beginGroup("batch");
  if (settings.contains("parallelBatchJobs")) {
    parallelBatchJobs = settings.value("parallelBatchJobs").toBool();
  } else {
    parallelBatchJobs = true;
  }
  if (settings.contains("skipFilesWithExistingTags")) {
    skipFilesWithExistingTags = settings.value("skipFilesWithExistingTags").toBool();
  } else {
    skipFilesWithExistingTags = false;
  }
  if (settings.contains("maxDuration")) {
    maxDuration = settings.value("maxDuration").toInt();
  } else {
    maxDuration = 60;
  }
  settings.endGroup();

  // ============================= Library ==================================

  settings.beginGroup("library");
  if (settings.contains("iTunesLibraryPath")) {
    iTunesLibraryPath = settings.value("iTunesLibraryPath").toString();
  } else {
#ifdef Q_OS_WIN
    iTunesLibraryPath = QDir::homePath() + "/My Music/iTunes/iTunes Music Library.xml";
#else
    iTunesLibraryPath = QDir::homePath() + "/Music/iTunes/iTunes Music Library.xml";
#endif
  }
  if (settings.contains("traktorLibraryPath")) {
    traktorLibraryPath = settings.value("traktorLibraryPath").toString();
  } else {
#ifdef Q_OS_WIN
    traktorLibraryPath = QDir::homePath() + "/My Documents/Native Instruments/Traktor 2.1.2/collection.nml";
#else
    traktorLibraryPath = QDir::homePath() + "/Documents/Native Instruments/Traktor 2.1.2/collection.nml";
#endif
  }
  if (settings.contains("seratoLibraryPath")) {
    seratoLibraryPath = settings.value("seratoLibraryPath").toString();
  } else {
#ifdef Q_OS_WIN
    seratoLibraryPath = QDir::homePath() + "/My Music/_Serato_/database V2";
#else
    seratoLibraryPath = QDir::homePath() + "/Music/_Serato_/database V2";
#endif
  }
  settings.endGroup();

  // ============================= UIState ==================================

  settings.beginGroup("uistate");
  if (settings.contains("batchWindowState")) {
    batchWindowState = settings.value("batchWindowState").toByteArray();
  } else {
    batchWindowState = QByteArray();
  }
  if (settings.contains("batchWindowGeometry")) {
    batchWindowGeometry = settings.value("batchWindowGeometry").toByteArray();
  } else {
    batchWindowGeometry = QByteArray();
  }
  if (settings.contains("batchWindowSplitterState")) {
    batchWindowSplitterState = settings.value("batchWindowSplitterState").toByteArray();
  } else {
    batchWindowSplitterState = QByteArray();
  }
  settings.endGroup();

  // ================================ End ======================================

  // default key codes
  defaultKeyCodes.push_back("A");   defaultKeyCodes.push_back("Am");
  defaultKeyCodes.push_back("Bb");  defaultKeyCodes.push_back("Bbm");
  defaultKeyCodes.push_back("B");   defaultKeyCodes.push_back("Bm");
  defaultKeyCodes.push_back("C");   defaultKeyCodes.push_back("Cm");
  defaultKeyCodes.push_back("Db");  defaultKeyCodes.push_back("Dbm");
  defaultKeyCodes.push_back("D");   defaultKeyCodes.push_back("Dm");
  defaultKeyCodes.push_back("Eb");  defaultKeyCodes.push_back("Ebm");
  defaultKeyCodes.push_back("E");   defaultKeyCodes.push_back("Em");
  defaultKeyCodes.push_back("F");   defaultKeyCodes.push_back("Fm");
  defaultKeyCodes.push_back("Gb");  defaultKeyCodes.push_back("Gbm");
  defaultKeyCodes.push_back("G");   defaultKeyCodes.push_back("Gm");
  defaultKeyCodes.push_back("Ab");  defaultKeyCodes.push_back("Abm");
  defaultKeyCodes.push_back("...");
  // key colours for display
  int a = 127;
  int b = 191;
  int c = 223;
  int d = 255;
  keyColours.push_back(qRgb(c,d,a)); keyColours.push_back(qRgb(b,c,a));
  keyColours.push_back(qRgb(d,a,d)); keyColours.push_back(qRgb(c,a,c));
  keyColours.push_back(qRgb(a,d,c)); keyColours.push_back(qRgb(a,c,b));
  keyColours.push_back(qRgb(d,a,a)); keyColours.push_back(qRgb(c,a,a));
  keyColours.push_back(qRgb(a,c,d)); keyColours.push_back(qRgb(a,b,c));
  keyColours.push_back(qRgb(d,d,a)); keyColours.push_back(qRgb(c,c,a));
  keyColours.push_back(qRgb(c,a,d)); keyColours.push_back(qRgb(b,a,c));
  keyColours.push_back(qRgb(a,d,a)); keyColours.push_back(qRgb(a,c,a));
  keyColours.push_back(qRgb(d,a,c)); keyColours.push_back(qRgb(c,a,b));
  keyColours.push_back(qRgb(a,d,d)); keyColours.push_back(qRgb(a,c,c));
  keyColours.push_back(qRgb(d,c,a)); keyColours.push_back(qRgb(c,b,a));
  keyColours.push_back(qRgb(a,a,c)); keyColours.push_back(qRgb(a,a,c));
  keyColours.push_back(qRgb(a,a,a));
}

void Preferences::save() {
  QSettings settings;

  settings.beginGroup("analysisFrequencies");
  settings.setValue("startingFrequencyA", core.getStartingFreqA());
  settings.setValue("numOctaves", core.getOctaves());
  settings.setValue("offsetToC", core.getOffsetToC());
  settings.setValue("bandsPerSemitone", core.getBandsPerOctave() / 12);
  settings.endGroup();

  settings.beginGroup("spectralAnalysis");
  settings.setValue("temporalWindow_1_14", core.getTemporalWindow());
  settings.setValue("fftFrameSize", core.getFftFrameSize());
  settings.setValue("hopsPerFrame", core.getHopsPerFrame());
  settings.setValue("directSkStretch", core.getDirectSkStretch());
  settings.endGroup();

  settings.beginGroup("tuning");
  settings.setValue("tuningMethod", core.getTuningMethod());
  settings.setValue("detunedBandWeight", core.getDetunedBandWeight());
  settings.endGroup();

  settings.beginGroup("harmonicChangeDetectionFunction");
  settings.setValue("segmentation", core.getSegmentation());
  settings.setValue("hcdfGaussianSize", core.getSegGaussianSize());
  settings.setValue("hcdfGaussianSigma", core.getSegGaussianSigma());
  settings.setValue("hcdfPeakPickingNeighbours", core.getSegPeakPickingNeighbours());
  settings.setValue("hcdfArbitrarySegments", core.getArbitrarySegments());
  settings.endGroup();

  settings.beginGroup("keyClassification");
  settings.setValue("toneProfile_1_14", core.getToneProfile());
  settings.setValue("similarityMeasure_1_14", core.getSimilarityMeasure());
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
  settings.setValue("A",    customKeyCodes[0]);   settings.setValue("Am",  customKeyCodes[1]);
  settings.setValue("Bb",   customKeyCodes[2]);   settings.setValue("Bbm", customKeyCodes[3]);
  settings.setValue("B",    customKeyCodes[4]);   settings.setValue("Bm",  customKeyCodes[5]);
  settings.setValue("C",    customKeyCodes[6]);   settings.setValue("Cm",  customKeyCodes[7]);
  settings.setValue("Db",   customKeyCodes[8]);   settings.setValue("Dbm", customKeyCodes[9]);
  settings.setValue("D",    customKeyCodes[10]);  settings.setValue("Dm",  customKeyCodes[11]);
  settings.setValue("Eb",   customKeyCodes[12]);  settings.setValue("Ebm", customKeyCodes[13]);
  settings.setValue("E",    customKeyCodes[14]);  settings.setValue("Em",  customKeyCodes[15]);
  settings.setValue("F",    customKeyCodes[16]);  settings.setValue("Fm",  customKeyCodes[17]);
  settings.setValue("Gb",   customKeyCodes[18]);  settings.setValue("Gbm", customKeyCodes[19]);
  settings.setValue("G",    customKeyCodes[20]);  settings.setValue("Gm",  customKeyCodes[21]);
  settings.setValue("Ab",   customKeyCodes[22]);  settings.setValue("Abm", customKeyCodes[23]);
  settings.setValue("SLNC", customKeyCodes[24]);
  settings.endGroup();

  settings.beginGroup("tags");
  settings.setValue("metadataFormat", metadataFormat);
  settings.setValue("metadataWriteTitle", metadataWriteTitle);
  settings.setValue("metadataWriteArtist", metadataWriteArtist);
  settings.setValue("metadataWriteAlbum", metadataWriteAlbum);
  settings.setValue("metadataWriteComment", metadataWriteComment);
  settings.setValue("metadataWriteGrouping", metadataWriteGrouping);
  settings.setValue("metadataWriteKey", metadataWriteKey);
  settings.setValue("metadataWriteFilename", metadataWriteFilename);
  settings.setValue("metadataDelimiter", metadataDelimiter);
  settings.setValue("writeToFilesAutomatically", writeToFilesAutomatically);
  settings.endGroup();

  settings.beginGroup("batch");
  settings.setValue("parallelBatchJobs", parallelBatchJobs);
  settings.setValue("skipFilesWithExistingTags", skipFilesWithExistingTags);
  settings.setValue("maxDuration", maxDuration);
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

Preferences& Preferences::operator=(const Preferences& that) {
  if (this != &that) {
    core = that.core;
    customKeyCodes            = that.customKeyCodes;
    metadataFormat            = that.metadataFormat;
    metadataWriteTitle        = that.metadataWriteTitle;
    metadataWriteArtist       = that.metadataWriteArtist;
    metadataWriteAlbum        = that.metadataWriteAlbum;
    metadataWriteComment      = that.metadataWriteComment;
    metadataWriteGrouping     = that.metadataWriteGrouping;
    metadataWriteKey          = that.metadataWriteKey;
    metadataWriteFilename     = that.metadataWriteFilename;
    metadataDelimiter         = that.metadataDelimiter;
    writeToFilesAutomatically = that.writeToFilesAutomatically;
    skipFilesWithExistingTags = that.skipFilesWithExistingTags;
    maxDuration               = that.maxDuration;
    parallelBatchJobs         = that.parallelBatchJobs;
    iTunesLibraryPath         = that.iTunesLibraryPath;
    traktorLibraryPath        = that.traktorLibraryPath;
    seratoLibraryPath         = that.seratoLibraryPath;
    batchWindowState          = that.batchWindowState;
    batchWindowGeometry       = that.batchWindowGeometry;
    batchWindowSplitterState  = that.batchWindowSplitterState;
  }
  return *this;
}

metadata_write_t Preferences::getMetadataWriteByTagEnum(metadata_tag_t tag) const {
  if      (tag == METADATA_TAG_TITLE)    return getMetadataWriteTitle();
  else if (tag == METADATA_TAG_ARTIST)   return getMetadataWriteArtist();
  else if (tag == METADATA_TAG_ALBUM)    return getMetadataWriteAlbum();
  else if (tag == METADATA_TAG_COMMENT)  return getMetadataWriteComment();
  else if (tag == METADATA_TAG_GROUPING) return getMetadataWriteGrouping();
  else if (tag == METADATA_TAG_KEY)      return getMetadataWriteKey();
  return METADATA_WRITE_NONE;
}

bool              Preferences::getWriteToFilesAutomatically() const { return writeToFilesAutomatically; }
bool              Preferences::getParallelBatchJobs()         const { return parallelBatchJobs; }
metadata_write_t  Preferences::getMetadataWriteTitle()        const { return metadataWriteTitle; }
metadata_write_t  Preferences::getMetadataWriteArtist()       const { return metadataWriteArtist; }
metadata_write_t  Preferences::getMetadataWriteAlbum()        const { return metadataWriteAlbum; }
metadata_write_t  Preferences::getMetadataWriteComment()      const { return metadataWriteComment; }
metadata_write_t  Preferences::getMetadataWriteGrouping()     const { return metadataWriteGrouping; }
metadata_write_t  Preferences::getMetadataWriteKey()          const { return metadataWriteKey; }
metadata_write_t  Preferences::getMetadataWriteFilename()     const { return metadataWriteFilename; }
metadata_format_t Preferences::getMetadataFormat()            const { return metadataFormat; }
bool              Preferences::getSkipFilesWithExistingTags() const { return skipFilesWithExistingTags; }
int               Preferences::getMaxDuration()               const { return maxDuration; }
QString           Preferences::getITunesLibraryPath()         const { return iTunesLibraryPath; }
QString           Preferences::getTraktorLibraryPath()        const { return traktorLibraryPath; }
QString           Preferences::getSeratoLibraryPath()         const { return seratoLibraryPath; }
QString           Preferences::getMetadataDelimiter()         const { return metadataDelimiter; }
QStringList       Preferences::getCustomKeyCodes()            const { return customKeyCodes; }
QByteArray        Preferences::getBatchWindowState()          const { return batchWindowState; }
QByteArray        Preferences::getBatchWindowGeometry()       const { return batchWindowGeometry; }
QByteArray        Preferences::getBatchWindowSplitterState()  const { return batchWindowSplitterState; }

bool                            Preferences::getOffsetToC()                const { return core.getOffsetToC(); }
KeyFinder::temporal_window_t    Preferences::getTemporalWindow()           const { return core.getTemporalWindow(); }
KeyFinder::segmentation_t       Preferences::getSegmentation()             const { return core.getSegmentation(); }
KeyFinder::similarity_measure_t Preferences::getSimilarityMeasure()        const { return core.getSimilarityMeasure(); }
KeyFinder::tuning_method_t      Preferences::getTuningMethod()             const { return core.getTuningMethod(); }
KeyFinder::tone_profile_t       Preferences::getToneProfile()              const { return core.getToneProfile(); }
unsigned int                    Preferences::getFftFrameSize()             const { return core.getFftFrameSize(); }
unsigned int                    Preferences::getHopsPerFrame()             const { return core.getHopsPerFrame(); }
unsigned int                    Preferences::getOctaves()                  const { return core.getOctaves(); }
unsigned int                    Preferences::getBpo()                      const { return core.getBandsPerOctave(); }
unsigned int                    Preferences::getArbitrarySegments()        const { return core.getArbitrarySegments(); }
unsigned int                    Preferences::getSegPeakPickingNeighbours() const { return core.getSegPeakPickingNeighbours(); }
unsigned int                    Preferences::getSegGaussianSize()          const { return core.getSegGaussianSize(); }
float                           Preferences::getSegGaussianSigma()         const { return core.getSegGaussianSigma(); }
float                           Preferences::getStartingFreqA()            const { return core.getStartingFreqA(); }
float                           Preferences::getLastFreq()                 const { return core.getLastFrequency(); }
float                           Preferences::getDirectSkStretch()          const { return core.getDirectSkStretch(); }
float                           Preferences::getDetunedBandWeight()        const { return core.getDetunedBandWeight(); }
std::vector<float>              Preferences::getCustomToneProfile()        const { return core.getCustomToneProfile(); }

void Preferences::setWriteToFilesAutomatically(bool autoWrite)     { writeToFilesAutomatically = autoWrite; }
void Preferences::setMetadataWriteTitle(metadata_write_t tit)      { metadataWriteTitle = tit; }
void Preferences::setMetadataWriteArtist(metadata_write_t art)     { metadataWriteArtist = art; }
void Preferences::setMetadataWriteAlbum(metadata_write_t alb)      { metadataWriteAlbum = alb; }
void Preferences::setMetadataWriteComment(metadata_write_t cmt)    { metadataWriteComment = cmt; }
void Preferences::setMetadataWriteGrouping(metadata_write_t grp)   { metadataWriteGrouping = grp; }
void Preferences::setMetadataWriteKey(metadata_write_t key)        { metadataWriteKey = key; }
void Preferences::setMetadataWriteFilename(metadata_write_t fn)    { metadataWriteFilename = fn; }
void Preferences::setSkipFilesWithExistingTags(bool skip)          { skipFilesWithExistingTags = skip; }
void Preferences::setMaxDuration(int max)                          { maxDuration = max; }
void Preferences::setMetadataFormat(metadata_format_t fmt)         { metadataFormat = fmt; }
void Preferences::setITunesLibraryPath(const QString& path)        { iTunesLibraryPath = path; }
void Preferences::setTraktorLibraryPath(const QString& path)       { traktorLibraryPath = path; }
void Preferences::setSeratoLibraryPath(const QString& path)        { seratoLibraryPath = path; }
void Preferences::setMetadataDelimiter(const QString & delim)      { metadataDelimiter = delim; }
void Preferences::setCustomKeyCodes(const QStringList& codes)      { customKeyCodes = codes; }
void Preferences::setBatchWindowState(const QByteArray& a)         { batchWindowState = a; }
void Preferences::setBatchWindowGeometry(const QByteArray& a)      { batchWindowGeometry = a; }
void Preferences::setBatchWindowSplitterState(const QByteArray& a) { batchWindowSplitterState = a; }

void Preferences::setOffsetToC(bool off)                                  { core.setOffsetToC(off); }
void Preferences::setTemporalWindow(KeyFinder::temporal_window_t w)       { core.setTemporalWindow(w); }
void Preferences::setSegmentation(KeyFinder::segmentation_t h)            { core.setSegmentation(h); }
void Preferences::setSimilarityMeasure(KeyFinder::similarity_measure_t m) { core.setSimilarityMeasure(m); }
void Preferences::setTuningMethod(KeyFinder::tuning_method_t t)           { core.setTuningMethod(t); }
void Preferences::setToneProfile(KeyFinder::tone_profile_t profile)       { core.setToneProfile(profile); }
void Preferences::setHopsPerFrame(unsigned int hpf)                       { core.setHopsPerFrame(hpf); }
void Preferences::setFftFrameSize(unsigned int framesize)                 { core.setFftFrameSize(framesize); }
void Preferences::setOctaves(unsigned int oct)                            { core.setOctaves(oct); }
void Preferences::setBps(unsigned int bands)                              { core.setBandsPerSemitone(bands); }
void Preferences::setArbitrarySegments(unsigned int segments)             { core.setArbitrarySegments(segments); }
void Preferences::setSegPeakPickingNeighbours(unsigned int neighbours)    { core.setSegPeakPickingNeighbours(neighbours); }
void Preferences::setSegGaussianSize(unsigned int size)                   { core.setSegGaussianSize(size); }
void Preferences::setSegGaussianSigma(float sigma)                        { core.setSegGaussianSigma(sigma); }
void Preferences::setStartingFreqA(float a)                               { core.setStartingFrequencyA(a); }
void Preferences::setDirectSkStretch(float stretch)                       { core.setDirectSkStretch(stretch); }
void Preferences::setDetunedBandWeight(float weight)                      { core.setDetunedBandWeight(weight); }
void Preferences::setCustomToneProfile(const std::vector<float>& profile) { core.setCustomToneProfile(profile); }


void Preferences::setParallelBatchJobs(bool parallel) {
  parallelBatchJobs = parallel;
  int numThreads = QThread::idealThreadCount();
  if (numThreads == -1 || parallel == false) {
    QThreadPool::globalInstance()->setMaxThreadCount(1);
  } else {
    QThreadPool::globalInstance()->setMaxThreadCount(numThreads);
  }
}

QString Preferences::getKeyCode(KeyFinder::key_t k) const {
  if (k < 0 || k >= defaultKeyCodes.size()) {
    qDebug("Attempt to get name of out-of-bounds key (%d/%d)",k,(int)defaultKeyCodes.size());
    return "";
  }
  QString defaultCode = defaultKeyCodes[k];
  QString customCode = customKeyCodes[k];
  if (metadataFormat == METADATA_FORMAT_KEYS || customCode == "")
    return defaultCode;
  else if (metadataFormat == METADATA_FORMAT_CUSTOM)
    return customCode;
  else
    return customCode + " " + defaultCode;
}

QStringList Preferences::getKeyCodeList() const {
  QStringList output;
  for (int i = 0; i < defaultKeyCodes.size(); i++)
    output << getKeyCode((KeyFinder::key_t)i);
  return output;
}

QColor Preferences::getKeyColour(KeyFinder::key_t k) const {
  if (k >= keyColours.size()) {
    qDebug("Attempt to get colour of out-of-bounds key (%d/%d)",k,(int)keyColours.size());
    return qRgb(0,0,0);
  }
  return keyColours[k];
}

void Preferences::setImageColours(QImage& image, chromagram_colour_t scheme) const {
  if (scheme == CHROMA_COLOUR_IZO) {
    for (int i=0; i<65; i++) {
      image.setColor(i,
        qRgb(
          scaledSine(i, 0.055, +4.50, 150, 130),
          scaledSine(i, 0.043, -1.80, 150, 150),
          scaledSine(i, 0.022, +4.70, 256, 270)
        )
      );
    }
  } else if (scheme == CHROMA_COLOUR_MONO) {
    for (int i=0; i<64; i++) {
      image.setColor(i,qRgb(i*4,i*4,i*4));
    }
    image.setColor(64,qRgb(255,255,255));
  } else if (scheme == CHROMA_COLOUR_PRED) {
    for (int i=0; i<65; i++) {
      image.setColor(i,
        qRgb(
          scaledSine(i, 0.077, -2.30, 320, 0),
          scaledSine(i, 0.077, -1.05, 320, 0),
          scaledSine(i, 0.077, +0.20, 320, 0)
        )
      );
    }
  } else if (scheme == CHROMA_COLOUR_VBOY) {
    for (int i=0; i<64; i++) {
      image.setColor(i,qRgb(i*4,0,0));
    }
    image.setColor(64,qRgb(255,0,0));
  } else {
    // Hack by default
    for (int i=0; i<64; i++) {
      image.setColor(i,qRgb(0,i*4,0));
    }
    image.setColor(64,qRgb(0,255,0));
  }
}

int Preferences::scaledSine(int index, float xScale, float xOffset, int yScale, int yOffset) const {
  int result = yScale * sin((index * xScale) + xOffset) + yOffset;
  if (result <   0) return 0;
  if (result > 255) return 255;
  return result;
}
