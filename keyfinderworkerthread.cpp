#include "keyfinderworkerthread.h"

// Declare the interthread communication types
Q_DECLARE_METATYPE(Chromagram)
Q_DECLARE_METATYPE(std::vector<double>)
Q_DECLARE_METATYPE(std::vector<int>)

KeyFinderWorkerThread::KeyFinderWorkerThread(const QString& f, const Preferences& p){
	prefs = p;
	filePath = f;
	// final registration of metatypes; not sure this is the best place for this
	qRegisterMetaType<Chromagram>("Chromagram");
	qRegisterMetaType<std::vector<double> >("std::vector<double>");
	qRegisterMetaType<std::vector<int> >("std::vector<int>");
}

void KeyFinderWorkerThread::run(){
	// initialise buffer and decode file into it
	AudioBuffer* ab = NULL;
	AudioFileDecoder* dec = AudioFileDecoder::getDecoder(filePath.toAscii().data());
	try{
		ab = dec->decodeFile(filePath.toAscii().data());
	}catch(Exception){
		delete ab;
		delete dec;
		emit failed("Could not decode file.");
		return;
	}
	delete dec;
	emit decoded();

	// make audio stream monaural
	Monaural* mono = new Monaural();
	ab = mono->makeMono(ab);
	delete mono;
	emit madeMono();

	// downsample if necessary
	if(prefs.getDFactor() > 1){
		Downsampler* ds = Downsampler::getDownsampler(prefs.getDFactor(),ab->getFrameRate(),prefs.getLastFreq());
		try{
			ab = ds->downsample(ab,prefs.getDFactor());
		}catch(Exception){
			delete ab;
			delete ds;
			emit failed("Downsampler failed.");
			return;
		}
		delete ds;
		emit downsampled();
	}

	// start spectrum analysis
	SpectrumAnalyser* sa = NULL;
	Chromagram* ch = NULL;
	sa = SpectrumAnalyserFactory::getInstance()->getSpectrumAnalyser(ab->getFrameRate(),prefs);
	ch = sa->chromagram(ab);
	delete ab;
	// note we don't delete the spectrum analyser; it stays in the centralised factory for reuse.
	emit producedFullChromagram(*ch);

	// decompose chromagram
	ch->decomposeToTwelveBpo(prefs);
	ch->decomposeToOneOctave(prefs);
	emit producedOneOctaveChromagram(*ch);

	// get energy level across track to weight segments
	std::vector<float> loudness(ch->getHops());
	for(int h=0; h<ch->getHops(); h++)
		for(int b=0; b<ch->getBins(); b++)
			loudness[h] += ch->getMagnitude(h,b);

	// get harmonic change signal
	Hcdf* hcdf = Hcdf::getHcdf(prefs);
	std::vector<double> harmonicChangeSignal = hcdf->hcdf(ch,prefs);
	emit producedHarmonicChangeSignal(harmonicChangeSignal);

	// get track segmentation
	std::vector<int> changes = hcdf->peaks(harmonicChangeSignal,prefs);
	changes.push_back(ch->getHops()-1);

	// batch output of keychange locations for Beatles experiment
	//for(int i=1; i<changes.size(); i++) // don't want the leading zero
	//	std::cout << filePath.substr(53) << "\t" << std::fixed << std::setprecision(2) << changes[i]*(prefs.getHopSize()/(44100.0/prefs.getDFactor())) << std::endl;
	// end experiment output

	// get key estimates for segments
	KeyClassifier hc(prefs);
	std::vector<int> keys(0);
	std::vector<float> keyWeights(24);
	for(int i=0; i<(signed)changes.size()-1; i++){
		std::vector<double> chroma(ch->getBins());
		for(int j=changes[i]; j<changes[i+1]; j++)
			for(int k=0; k<ch->getBins(); k++)
				chroma[k] += ch->getMagnitude(j,k);
		int key = hc.classify(chroma);
		for(int j=changes[i]; j<changes[i+1]; j++){
			keys.push_back(key);
			if(key < 24) // ignore parts that were classified as silent
				keyWeights[key] += loudness[j];
		}
	}
	keys.push_back(keys[keys.size()-1]); // put last key on again to match length of track
	delete ch;
	emit producedKeyEstimates(keys);

	// get global key
	int mostCommonKey = -1;
	float mostCommonKeyWeight = 0.0;
	for(int i=0; i<(signed)keyWeights.size(); i++){
		if(keyWeights[i] > mostCommonKeyWeight){
			mostCommonKeyWeight = keyWeights[i];
			mostCommonKey = i;
		}
	}
	emit producedGlobalKeyEstimate(mostCommonKey);
	return;
}
