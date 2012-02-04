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

	settings.beginGroup("analysisFrequencies");
	if(settings.contains("startingFrequencyA")){
		stFreq = settings.value("startingFrequencyA").toFloat();
	}else{
		float defaultVal = 27.5;
		settings.setValue("startingFrequencyA",defaultVal);
		stFreq = defaultVal;
		qDebug("Wrote default startingFrequencyA (%f)",stFreq);
	}
	if(settings.contains("numOctaves")){
		octaves = settings.value("numOctaves").toInt();
	}else{
		int defaultVal = 6;
		settings.setValue("numOctaves",defaultVal);
		octaves = defaultVal;
		qDebug("Wrote default numOctaves (%d)",octaves);
	}
  // 3 gets an octave that starts at C.
	if(settings.contains("octaveBeginOffset")){
		octaveOffset = settings.value("octaveBeginOffset").toInt();
	}else{
    int defaultVal = 3;
		settings.setValue("octaveBeginOffset",defaultVal);
		octaveOffset = defaultVal;
		qDebug("Wrote default octaveBeginOffset (%d)",octaveOffset);
	}
  // >1 bps has only reduced accuracy, all other things being equal. default 1.
	if(settings.contains("bandsPerSemitone")){
		bps = settings.value("bandsPerSemitone").toInt();
	}else{
		int defaultVal = 1;
		settings.setValue("bandsPerSemitone",defaultVal);
		bps = defaultVal;
		qDebug("Wrote default bandsPerSemitone (%d)",bps);
	}
	settings.endGroup();

	// ======================== Spectral analysis ================================

	settings.beginGroup("spectralAnalysis");
	if(settings.contains("temporalWindow")){
    temporalWindow = settings.value("temporalWindow").toChar().toAscii();
	}else{
		char defaultVal = 'b';
		settings.setValue("temporalWindow",defaultVal);
		temporalWindow = defaultVal;
		qDebug("Wrote default temporalWindow (%c)",temporalWindow);
	}
  int defFrameSize = 16384;
	if(settings.contains("fftFrameSize")){
		fftFrameSize = settings.value("fftFrameSize").toInt();
	}else{
		settings.setValue("fftFrameSize",defFrameSize);
		fftFrameSize = defFrameSize;
		qDebug("Wrote default fftFrameSize (%d)",fftFrameSize);
	}
	int defHopSize = defFrameSize / 4;
	if(settings.contains("hopSize")){
		hopSize = settings.value("hopSize").toInt();
	}else{
		settings.setValue("hopSize",defHopSize);
		hopSize = defHopSize;
		qDebug("Wrote default hopSize (%d)",hopSize);
	}
	if(settings.contains("directSkStretch")){
		directSkStretch = settings.value("directSkStretch").toFloat();
	}else{
    float defaultVal = 0.8;
		settings.setValue("directSkStretch",defaultVal);
		directSkStretch = defaultVal;
		qDebug("Wrote default directSkStretch (%f)",directSkStretch);
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
		tuningMethod = settings.value("tuningMethod").toInt();
	}else{
		int defaultVal = 0;
		settings.setValue("tuningMethod",defaultVal);
		tuningMethod = defaultVal;
		qDebug("Wrote default tuningMethod (%d)",tuningMethod);
	}
	if(settings.contains("detunedBandWeight")){
		detunedBandWeight = settings.value("detunedBandWeight").toFloat();
	}else{
    float defaultVal = 0.2;
		settings.setValue("detunedBandWeight",defaultVal);
		detunedBandWeight = defaultVal;
		qDebug("Wrote default detunedBandWeight (%f)",detunedBandWeight);
	}
	settings.endGroup();

	// ================ Harmonic Change Detection Function =======================

	settings.beginGroup("harmonicChangeDetectionFunction");
	if(settings.contains("hcdf")){
    hcdf = settings.value("hcdf").toChar().toAscii();
	}else{
		char defaultVal = 'n';
		settings.setValue("hcdf",defaultVal);
		hcdf = defaultVal;
		qDebug("Wrote default hcdf (%c)",hcdf);
	}
	if(settings.contains("hcdfGaussianSize")){
		hcdfGaussianSize = settings.value("hcdfGaussianSize").toInt();
	}else{
		int defaultVal = 35;
		settings.setValue("hcdfGaussianSize",defaultVal);
		hcdfGaussianSize = defaultVal;
		qDebug("Wrote default hcdfGaussianSize (%d)",hcdfGaussianSize);
	}
	if(settings.contains("hcdfGaussianSigma")){
		hcdfGaussianSigma = settings.value("hcdfGaussianSigma").toFloat();
	}else{
		float defaultVal = 8.0;
		settings.setValue("hcdfGaussianSigma",defaultVal);
		hcdfGaussianSigma = defaultVal;
		qDebug("Wrote default hcdfGaussianSigma (%f)",hcdfGaussianSigma);
	}
	if(settings.contains("hcdfPeakPickingNeighbours")){
		hcdfPeakPickingNeighbours = settings.value("hcdfPeakPickingNeighbours").toInt();
	}else{
		int defaultVal = 4;
		settings.setValue("hcdfPeakPickingNeighbours",defaultVal);
		hcdfPeakPickingNeighbours = defaultVal;
		qDebug("Wrote default hcdfPeakPickingNeighbours (%d)",hcdfPeakPickingNeighbours);
	}
	if(settings.contains("hcdfArbitrarySegments")){
		hcdfArbitrarySegments = settings.value("hcdfArbitrarySegments").toInt();
	}else{
		int defaultVal = 3;
		settings.setValue("hcdfArbitrarySegments",defaultVal);
		hcdfArbitrarySegments = defaultVal;
		qDebug("Wrote default hcdfArbitrarySegments (%d)",hcdfArbitrarySegments);
	}
	settings.endGroup();

	// ========================= Key classification ==============================

	settings.beginGroup("keyClassification");
	if(settings.contains("toneProfile")){
		toneProfile = settings.value("toneProfile").toInt();
	}else{
    int defaultVal = 3;
		settings.setValue("toneProfile",defaultVal);
		toneProfile = defaultVal;
		qDebug("Wrote default toneProfile (%d)",toneProfile);
	}
	if(settings.contains("similarityMeasure")){
    similarityMeasure = settings.value("similarityMeasure").toChar().toAscii();
	}else{
		char defaultVal = 'c';
		settings.setValue("similarityMeasure",defaultVal);
		similarityMeasure = defaultVal;
		qDebug("Wrote default similarityMeasure (%c)",similarityMeasure);
	}
	settings.endGroup();

	// ========================= Custom Tone Profile ==============================

	settings.beginGroup("customToneProfile");
	if(settings.contains("maj0")){
		customToneProfile = std::vector<float>(0);
		customToneProfile.push_back(settings.value("maj0").toFloat());
		customToneProfile.push_back(settings.value("maj1").toFloat());
		customToneProfile.push_back(settings.value("maj2").toFloat());
		customToneProfile.push_back(settings.value("maj3").toFloat());
		customToneProfile.push_back(settings.value("maj4").toFloat());
		customToneProfile.push_back(settings.value("maj5").toFloat());
		customToneProfile.push_back(settings.value("maj6").toFloat());
		customToneProfile.push_back(settings.value("maj7").toFloat());
		customToneProfile.push_back(settings.value("maj8").toFloat());
		customToneProfile.push_back(settings.value("maj9").toFloat());
		customToneProfile.push_back(settings.value("maj10").toFloat());
		customToneProfile.push_back(settings.value("maj11").toFloat());
		customToneProfile.push_back(settings.value("min0").toFloat());
		customToneProfile.push_back(settings.value("min1").toFloat());
		customToneProfile.push_back(settings.value("min2").toFloat());
		customToneProfile.push_back(settings.value("min3").toFloat());
		customToneProfile.push_back(settings.value("min4").toFloat());
		customToneProfile.push_back(settings.value("min5").toFloat());
		customToneProfile.push_back(settings.value("min6").toFloat());
		customToneProfile.push_back(settings.value("min7").toFloat());
		customToneProfile.push_back(settings.value("min8").toFloat());
		customToneProfile.push_back(settings.value("min9").toFloat());
		customToneProfile.push_back(settings.value("min10").toFloat());
		customToneProfile.push_back(settings.value("min11").toFloat());
	}else{
		float defaultValues[24] = {
			1,0,1,0,1,1,0,1,0,1,0,1,
			1,0,1,1,0,1,0,1,1,0,1,0
		};
		customToneProfile = std::vector<float>(defaultValues, defaultValues+23);
		settings.setValue("maj0",customToneProfile[0]);
		settings.setValue("maj1",customToneProfile[1]);
		settings.setValue("maj2",customToneProfile[2]);
		settings.setValue("maj3",customToneProfile[3]);
		settings.setValue("maj4",customToneProfile[4]);
		settings.setValue("maj5",customToneProfile[5]);
		settings.setValue("maj6",customToneProfile[6]);
		settings.setValue("maj7",customToneProfile[7]);
		settings.setValue("maj8",customToneProfile[8]);
		settings.setValue("maj9",customToneProfile[9]);
		settings.setValue("maj10",customToneProfile[10]);
		settings.setValue("maj11",customToneProfile[11]);
		settings.setValue("min0",customToneProfile[12]);
		settings.setValue("min1",customToneProfile[13]);
		settings.setValue("min2",customToneProfile[14]);
		settings.setValue("min3",customToneProfile[15]);
		settings.setValue("min4",customToneProfile[16]);
		settings.setValue("min5",customToneProfile[17]);
		settings.setValue("min6",customToneProfile[18]);
		settings.setValue("min7",customToneProfile[19]);
		settings.setValue("min8",customToneProfile[20]);
		settings.setValue("min9",customToneProfile[21]);
		settings.setValue("min10",customToneProfile[22]);
		settings.setValue("min11",customToneProfile[23]);
		qDebug("Wrote default customToneProfile");
	}
	settings.endGroup();

	// ========================== Custom Key Codes ===============================

	settings.beginGroup("customKeyCodes");
	if(settings.contains("majKey0")){
		customKeyCodes = std::vector<QString>(0);
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
		customKeyCodes = std::vector<QString>(25); // default to empty strings
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
    tagFormat = settings.value("tagFormat").toChar().toAscii();
  }else{
    char defaultVal = 'k';
    settings.setValue("tagFormat",defaultVal);
    tagFormat = defaultVal;
    qDebug("Wrote default tagFormat (%c)",tagFormat);
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

  // ========================= Batch jobs ==============================

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

	// ================================ End ======================================

	generateBinFreqs();

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

Preferences& Preferences::operator=(const Preferences& that){
	if(this != &that){
		temporalWindow = that.temporalWindow;
		hcdf = that.hcdf;
		hopSize = that.hopSize;
		fftFrameSize = that.fftFrameSize;
		octaves = that.octaves;
		bps = that.bps;
		octaveOffset = that.octaveOffset;
		dFactor = that.dFactor;
		toneProfile = that.toneProfile;
		similarityMeasure = that.similarityMeasure;
		hcdfPeakPickingNeighbours = that.hcdfPeakPickingNeighbours;
		hcdfArbitrarySegments = that.hcdfArbitrarySegments;
		hcdfGaussianSize = that.hcdfGaussianSize;
		tuningMethod = that.tuningMethod;
		hcdfGaussianSigma = that.hcdfGaussianSigma;
		stFreq = that.stFreq;
		directSkStretch = that.directSkStretch;
		detunedBandWeight = that.detunedBandWeight;
		customToneProfile = that.customToneProfile;
		customKeyCodes = that.customKeyCodes;
    tagFormat = that.tagFormat;
    writeToTagComment = that.writeToTagComment;
    writeToTagGrouping = that.writeToTagGrouping;
    writeToTagKey = that.writeToTagKey;
    writeTagsAutomatically = that.writeTagsAutomatically;
    skipFilesWithExistingTags = that.skipFilesWithExistingTags;
    parallelBatchJobs = that.parallelBatchJobs;
		generateBinFreqs();
	}
	return *this;
}


bool Preferences::equivalentSpectralAnalysis(const Preferences& that) const{
	if(temporalWindow != that.temporalWindow)
		return false;
	if(bps != that.bps)
		return false;
	if(stFreq != that.stFreq)
		return false;
	if(octaves != that.octaves)
		return false;
	if(octaveOffset != that.octaveOffset)
		return false;
  if(fftFrameSize != that.fftFrameSize)
    return false;
  if(directSkStretch != that.directSkStretch)
    return false;
	return true;
}

bool  Preferences::getWriteTagsAutomatically()    const { return writeTagsAutomatically; }
bool  Preferences::getParallelBatchJobs()         const { return parallelBatchJobs; }
bool  Preferences::getWriteToTagComment()         const { return writeToTagComment; }
bool  Preferences::getWriteToTagGrouping()        const { return writeToTagGrouping; }
bool  Preferences::getWriteToTagKey()             const { return writeToTagKey; }
bool  Preferences::getSkipFilesWithExistingTags() const { return skipFilesWithExistingTags; }
char  Preferences::getTemporalWindow()            const { return temporalWindow; }
char  Preferences::getHcdf()                      const { return hcdf; }
char  Preferences::getSimilarityMeasure()         const { return similarityMeasure; }
char  Preferences::getTagFormat()                 const { return tagFormat; }
int   Preferences::getHopSize()                   const { return hopSize; }
int   Preferences::getFftFrameSize()              const { return fftFrameSize; }
int   Preferences::getOctaves()                   const { return octaves; }
int   Preferences::getBpo()                       const { return bps * 12; }
int   Preferences::getOctaveOffset()              const { return octaveOffset; }
int   Preferences::getDFactor()                   const { return dFactor; }
int   Preferences::getToneProfile()               const { return toneProfile; }
int   Preferences::getTuningMethod()              const { return tuningMethod; }
int   Preferences::getHcdfPeakPickingNeighbours() const { return hcdfPeakPickingNeighbours; }
int   Preferences::getHcdfArbitrarySegments()     const { return hcdfArbitrarySegments; }
int   Preferences::getHcdfGaussianSize()          const { return hcdfGaussianSize; }
float Preferences::getHcdfGaussianSigma()         const { return hcdfGaussianSigma; }
float Preferences::getDirectSkStretch()           const { return directSkStretch; }
float Preferences::getDetunedBandWeight()         const { return detunedBandWeight; }

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

std::vector<float> Preferences::getCustomToneProfile() const{
	return customToneProfile;
}

std::vector<QString> Preferences::getCustomKeyCodes() const{
	return customKeyCodes;
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
	binFreqs = std::vector<float>(octaves*bpo);
	float freqRatio = pow(2,(1.0/bpo));
	float octFreq = stFreq;
	float binFreq;
	int concertPitchBin = bps/2;
	for(int i=0; i<octaves; i++){
		binFreq = octFreq;
		// offset by specified number of semitones
		for(int j=0; j<octaveOffset*bps; j++)
			binFreq *= freqRatio;
		// tune down for bins before first concert pitch bin (if bps > 1)
		for(int j=0; j<concertPitchBin; j++)
			binFreqs[(i*bpo)+j] = binFreq / pow(freqRatio,(concertPitchBin-j));
		// and tune all other bins
		for(int j=concertPitchBin; j<bpo; j++){
			binFreqs[(i*bpo)+j] = binFreq;
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
