#include "fftwanalyser.h"

FftwAnalyser::FftwAnalyser(int f, const Preferences& prefs): SpectrumAnalyser(f, prefs) {
	pp = NULL;
	fftFrameSize = prefs.getFftFrameSize();
	if(prefs.getFftPostProcessor() == 'c'){
		pp = new ConstantQPostProc(frameRate, prefs);
	}else if(prefs.getFftPostProcessor() == 'l'){
		pp = new LogBinsPostProc(frameRate, prefs);
	}else{
		pp = new DirectSkPostProc(frameRate, prefs);
	}
	//pp->printKernel();
	fftInput = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*fftFrameSize);
	fftResult = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*fftFrameSize);
	// plan 1-dimensional DFT
	fftPlan = fftw_plan_dft_1d(fftFrameSize, fftInput, fftResult, FFTW_FORWARD, FFTW_ESTIMATE);
	// prep temporal window function
	WindowFunction* wf = WindowFunction::getWindowFunction(prefs.getTemporalWindow());
	window = std::vector<float>(fftFrameSize);
	for(int i=0; i<fftFrameSize; i++){
		window[i] = wf->window(i,fftFrameSize);
	}
}

FftwAnalyser::~FftwAnalyser(){
	delete pp;
	fftw_destroy_plan(fftPlan);
	fftw_free(fftInput);
	fftw_free(fftResult);
}

Chromagram* FftwAnalyser::chromagram(AudioBuffer* ab){
	QMutexLocker locker(&mutex); // Mutex this function
	Chromagram* ch = new Chromagram((ab->getSampleCount()/hopSize) + 1,bins);
	for(int i=0; i<ab->getSampleCount(); i += hopSize){
		for(int j=0; j<fftFrameSize; j++){
			if(i+j < ab->getSampleCount())
				fftInput[j][0] = ab->getSample(i+j) * window[j]; // real part, windowed
			else
				fftInput[j][0] = 0.0; // zero-pad if no PCM data remaining
			fftInput[j][1] = 0.0; // zero out imaginary part
		}
		fftw_execute(fftPlan);
		std::vector<float> cv = pp->chromaVector(fftResult);
		for(int j=0; j<bins; j++)
			ch->setMagnitude(i/hopSize,j,cv[j]);
	}
	return ch;
}
