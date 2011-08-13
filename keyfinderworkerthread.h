#ifndef KEYFINDERMODEL_H
#define KEYFINDERMODEL_H

#include <QObject>
#include <QThread>
#include <QString>

#include <vector>

#include "preferences.h"
#include "exception.h"
#include "audiobuffer.h"
#include "decoder.h"
#include "monaural.h"
#include "downsampler.h"
#include "chromagram.h"
#include "spectrumanalyserfactory.h"
#include "hcdf.h"
#include "keyclassifier.h"

class KeyFinderWorkerThread : public QThread{
Q_OBJECT
public:
	KeyFinderWorkerThread(const QString&, const Preferences&);
protected:
	void run();
signals:
	void failed(const QString&);
	void decoded();
	void madeMono();
	void downsampled();
	void producedFullChromagram(const Chromagram&);
	void producedOneOctaveChromagram(const Chromagram&);
	void producedHarmonicChangeSignal(const std::vector<double>&);
	void producedKeyEstimates(const std::vector<int>&);
	void producedGlobalKeyEstimate(int);
private:
	Preferences prefs;
	QString filePath;
};

#endif // KEYFINDERMODEL_H
