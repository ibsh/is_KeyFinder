#include <QtCore/QString>
#include <QtTest/QtTest>
#include "unittest.h"

#include "decoderlibav.h"
#include "exception.h"

class LibAvDecoder_Test : public QObject{
	Q_OBJECT
public:
	LibAvDecoder_Test();
private Q_SLOTS:
	void testDecodeWav();
	void testDecodeFlac();
	void testDecodeAlac();
	void testDecodeMp3();
};
DECLARE_TEST(LibAvDecoder_Test)

LibAvDecoder_Test::LibAvDecoder_Test(){}

void LibAvDecoder_Test::testDecodeWav(){
	LibAvDecoder* dec = new LibAvDecoder;
	AudioBuffer* ab = NULL;
	try{
		ab = dec->decodeFile("../is_KeyFinder/_testaudio/audio-441-sq.wav");
	}catch(...){
		QFAIL("Decoder failed");
	}
	delete dec;
	/*
		441-sq.wav contains 1 monophonic second of unity-magnitude square wave at 441Hz,
		so we can test the accuracy of the decode operation
	*/
	QVERIFY(ab->getFrameRate() == 44100);
	QVERIFY(ab->getSampleCount() == 44100);
	QVERIFY(ab->getChannels() == 1);
	// allow a very small margin for error (32760 vs 32768)
	QVERIFY(ab->getSample(0) > 32760);
	QVERIFY(ab->getSample(49) > 32760);
	QVERIFY(ab->getSample(50) < -32760);
	QVERIFY(ab->getSample(99) < -32760);
	QVERIFY(ab->getSample(100) > 32760);
	delete ab;
}

void LibAvDecoder_Test::testDecodeFlac(){
	LibAvDecoder* dec = new LibAvDecoder;
	AudioBuffer* ab = NULL;
	try{
		ab = dec->decodeFile("../is_KeyFinder/_testaudio/audio-441-sq.flac");
	}catch(...){
		QFAIL("Decoder failed");
	}
	delete dec;
	QVERIFY(ab->getFrameRate() == 44100);
	QVERIFY(ab->getSampleCount() == 44100);
	QVERIFY(ab->getChannels() == 1);
	QVERIFY(ab->getSample(49) > 32760);
	QVERIFY(ab->getSample(50) < -32760);
	delete ab;
}

void LibAvDecoder_Test::testDecodeAlac(){
	LibAvDecoder* dec = new LibAvDecoder;
	AudioBuffer* ab = NULL;
	try{
		ab = dec->decodeFile("../is_KeyFinder/_testaudio/audio-441-sq.m4a");
	}catch(...){
		QFAIL("Decoder failed");
	}
	delete dec;
	QVERIFY(ab->getFrameRate() == 44100);
	QVERIFY(ab->getSampleCount() == 44100);
	QVERIFY(ab->getChannels() == 1);
	QVERIFY(ab->getSample(49) > 32760);
	QVERIFY(ab->getSample(50) < -32760);
	delete ab;
}

void LibAvDecoder_Test::testDecodeMp3(){
	LibAvDecoder* dec = new LibAvDecoder;
	AudioBuffer* ab = NULL;
	try{
		ab = dec->decodeFile("../is_KeyFinder/_testaudio/audio-441-sq.mp3");
	}catch(...){
		QFAIL("Decoder failed");
	}
	delete dec;
	// An MP3 is not an exact copy so some extra samples are added etc. Not much point testing in depth.
	QVERIFY(ab->getFrameRate() == 44100);
	QVERIFY(ab->getSampleCount() >= 44100);
	QVERIFY(ab->getChannels() == 1);
	delete ab;
}

#include "test_decoderlibav.moc"

