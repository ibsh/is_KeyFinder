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
	// test that exception handling for bad alloc
	try{
		ab->addSamples(INT_MAX);
		QFAIL("Either your machine has a startlingly large memory or an exception failed to throw.");
	}catch(...){
		QVERIFY(true);
	}
	delete ab;
}

#include "test_audiobuffer.moc"
