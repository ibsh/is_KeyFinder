#include <QtCore/QString>
#include <QtTest/QtTest>
#include "unittest.h"

#include "decoderlibav.h"
#include "downsamplerib.h"
#include "exception.h"

class IbDownsampler_Test : public QObject{
	Q_OBJECT
public:
	IbDownsampler_Test();
private Q_SLOTS:
	void testDownsample();
};
DECLARE_TEST(IbDownsampler_Test)

IbDownsampler_Test::IbDownsampler_Test(){}

void IbDownsampler_Test::testDownsample(){
	LibAvDecoder* dec = new LibAvDecoder;
	AudioBuffer* ab = NULL;
	try{
		ab = dec->decodeFile("../is_KeyFinder/_testaudio/audio-441-sine.wav");
	}catch(...){
		QFAIL("Decoder failed");
	}
	delete dec;
	/*
		441-sine.wav contains 1 monophonic second of unity-magnitude sine wave at 441Hz,
		so we can downsample by a factor of 10 and then test the waveform.
	*/
	IbDownsampler* ds = new IbDownsampler();
	ab = ds->downsample(ab,10);
	QVERIFY(ab->getFrameRate() == 4410);
	QVERIFY(ab->getSampleCount() == 4410);
	QVERIFY(ab->getChannels() == 1);
	QVERIFY(ab->getSample(4) > 32768);
	QVERIFY(ab->getSample(9) < -32768);
	QVERIFY(ab->getSample(14) > 32768);
	delete ab;
}

#include "test_downsamplerib.moc"


