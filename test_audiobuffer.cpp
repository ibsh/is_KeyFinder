#include <QtCore/QString>
#include <QtTest/QtTest>
#include "unittest.h"

#include "audiobuffer.h"

class AudioBuffer_Test : public QObject{
	Q_OBJECT
public:
	AudioBuffer_Test();
private Q_SLOTS:
	void testConstructor();
	void testMutators();
};
DECLARE_TEST(AudioBuffer_Test)

AudioBuffer_Test::AudioBuffer_Test(){}

void AudioBuffer_Test::testConstructor(){
	AudioBuffer* ab = new AudioBuffer();
	QVERIFY(ab->getChannels() == 0);
	QVERIFY(ab->getFrameRate() == 0);
	QVERIFY(ab->getSampleCount() == 0);
	delete ab;
}

void AudioBuffer_Test::testMutators(){
	AudioBuffer* ab = new AudioBuffer();
	ab->setChannels(2);
	QVERIFY(ab->getChannels() == 2);
	ab->setFrameRate(44100);
	QVERIFY(ab->getFrameRate() == 44100);
	ab->addSamples(65536);
	QVERIFY(ab->getSampleCount() == 65536);
	QVERIFY(ab->getSample(0) == 0.0);
	ab->setSample(101,16384.0);
	QVERIFY(ab->getSample(101) == 16384.0);
	delete ab;
}

#include "test_audiobuffer.moc"
