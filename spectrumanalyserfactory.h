#ifndef SPECTRUMANALYSERFACTORY_H
#define SPECTRUMANALYSERFACTORY_H

#include <QMutexLocker>

#include "spectrumanalyser.h"
#include "goertzelanalyser.h"
#include "fftwanalyser.h"
#include "preferences.h"

#include <vector>

// Keeps a reference to a spectrum analyser with distinguishing information
class SpectrumAnalyserWrapper{
public:
	SpectrumAnalyserWrapper(char,char,int,char,SpectrumAnalyser*);
	~SpectrumAnalyserWrapper();
	char getType() const;
	char getFftPostProcessor() const;
	int getFramerate() const;
	char getTemporalWindow() const;
	SpectrumAnalyser* getSpectrumAnalyser() const;
private:
	char type;
	char fftpp;
	int frate;
	char window;
	SpectrumAnalyser* sa;
};

// Singleton. It holds all analysers generated in a session, to cut down on prep time.
class SpectrumAnalyserFactory{
public:
	SpectrumAnalyser* getSpectrumAnalyser(int, const Preferences&);
	static SpectrumAnalyserFactory* getInstance();
	~SpectrumAnalyserFactory();
private:
	mutable QMutex mutex; // used to make getSpectrumAnalyser thread-safe
	SpectrumAnalyserFactory();
	static SpectrumAnalyserFactory* inst;
	std::vector<SpectrumAnalyserWrapper*> analysers;
};

#endif // SPECTRUMANALYSERFACTORY_H
