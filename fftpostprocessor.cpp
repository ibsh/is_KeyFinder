#include "fftpostprocessor.h"

FftPostProcessor::FftPostProcessor(int f, const Preferences& prefs){
	frameRate = f;
	bins = prefs.getOctaves() * prefs.getBpo();
	fftFrameSize = prefs.getFftFrameSize();
}
