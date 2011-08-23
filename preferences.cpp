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
	// 3 gets an octave that starts at C, but accuracy drops a little.
	if(settings.contains("octaveBeginOffset")){
		octaveOffset = settings.value("octaveBeginOffset").toInt();
	}else{
		int defaultVal = 0;
		settings.setValue("octaveBeginOffset",defaultVal);
		octaveOffset = defaultVal;
		qDebug("Wrote default octaveBeginOffset (%d)",octaveOffset);
	}
	// 3 bps has only reduced accuracy, all other things being equal. default 1.
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
	if(settings.contains("spectrumAnalyser")){
		spectrumAnalyser = settings.value("spectrumAnalyser").toChar().toAscii();
	}else{
		char defaultVal = 'f';
		settings.setValue("spectrumAnalyser",defaultVal);
		spectrumAnalyser = defaultVal;
		qDebug("Wrote default spectrumAnalyser (%c)",spectrumAnalyser);
	}
	if(settings.contains("fftPostProcessor")){
		fftPostProcessor = settings.value("fftPostProcessor").toChar().toAscii();
	}else{
		char defaultVal = 'i';
		settings.setValue("fftPostProcessor",defaultVal);
		fftPostProcessor = defaultVal;
		qDebug("Wrote default fftPostProcessor (%c)",fftPostProcessor);
	}
	int defFrameSize = 65536;
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
	// 3.8 closely models the CQT, 1.0 is much tighter and apparently more accurate.
	if(settings.contains("directSkStretch")){
		directSkStretch = settings.value("directSkStretch").toFloat();
	}else{
		float defaultVal = 1.0;
		settings.setValue("directSkStretch",defaultVal);
		directSkStretch = defaultVal;
		qDebug("Wrote default directSkStretch (%f)",directSkStretch);
	}
	if(settings.contains("goertzelMinK")){
		goertzelMinK = settings.value("goertzelMinK").toInt();
	}else{
		int defaultVal = 60;
		settings.setValue("goertzelMinK",defaultVal);
		goertzelMinK = defaultVal;
		qDebug("Wrote default goertzelMinK (%d)",goertzelMinK);
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
		float defaultVal = 0.5;
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
		int defaultVal = 2;
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
		qDebug("Wrote default (blank) customKeyCodes");
	}
	settings.endGroup();

	// ================================ End ======================================

	generateBinFreqs();

}

Preferences& Preferences::operator=(const Preferences& that){
	if(this != &that){
		spectrumAnalyser = that.spectrumAnalyser;
		fftPostProcessor = that.fftPostProcessor;
		temporalWindow = that.temporalWindow;
		hcdf = that.hcdf;
		hopSize = that.hopSize;
		fftFrameSize = that.fftFrameSize;
		goertzelMinK = that.goertzelMinK;
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
	if(spectrumAnalyser != that.spectrumAnalyser)
		return false;
	else{
		if(spectrumAnalyser == 'f'){
			if(fftFrameSize != that.fftFrameSize)
				return false;
			if(fftPostProcessor != that.fftPostProcessor)
				return false;
			else{
				if(fftPostProcessor == 'i'){
					if(directSkStretch != that.directSkStretch)
						return false;
				}
			}
		}else{
			if(goertzelMinK != that.goertzelMinK)
			 return false;
		}
	}
	return true;
}

// These mutators are only used by unit tests at present

void Preferences::setFftPostProcessor(char ch){
	if (ch == 'i' || ch == 'c' || ch == 'l')
		fftPostProcessor = ch;
}

void Preferences::setBandsPerSemitone(int n){
	if(n%2 != 0){
		bps = n;
		generateBinFreqs();
	}
}

void Preferences::setDownsampleFactor(int n){
	dFactor = n;
}

void Preferences::setTuningMethod(int n){
	if(n >= 0 && n <= 1)
		tuningMethod = n;
}

char Preferences::getSpectrumAnalyser()const{return spectrumAnalyser;}
char Preferences::getFftPostProcessor()const{return fftPostProcessor;}
char Preferences::getTemporalWindow()const{return temporalWindow;}
char Preferences::getHcdf()const{return hcdf;}
char Preferences::getSimilarityMeasure()const{return similarityMeasure;}
int Preferences::getHopSize()const{return hopSize;}
int Preferences::getFftFrameSize()const{return fftFrameSize;}
int Preferences::getGoertzelMinK()const{return goertzelMinK;}
int Preferences::getOctaves()const{return octaves;}
int Preferences::getBpo()const{return bps * 12;}
int Preferences::getOctaveOffset()const{return octaveOffset;}
int Preferences::getDFactor()const{return dFactor;}
int Preferences::getToneProfile()const{return toneProfile;}
int Preferences::getTuningMethod()const{return tuningMethod;}
int Preferences::getHcdfPeakPickingNeighbours()const{return hcdfPeakPickingNeighbours;}
int Preferences::getHcdfArbitrarySegments()const{return hcdfArbitrarySegments;}
int Preferences::getHcdfGaussianSize()const{return hcdfGaussianSize;}
float Preferences::getHcdfGaussianSigma()const{return hcdfGaussianSigma;}
float Preferences::getDirectSkStretch()const{return directSkStretch;}
float Preferences::getDetunedBandWeight()const{return detunedBandWeight;}

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
