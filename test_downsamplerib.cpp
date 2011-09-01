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
	AudioStream* ab = NULL;
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
	PrimaryDownsampler* ds = new PrimaryDownsampler();
	ab = ds->downsample(ab,10);
	QVERIFY(ab->getFrameRate() == 4410);
	QVERIFY(ab->getSampleCount() == 4410);
	QVERIFY(ab->getChannels() == 1);
	QVERIFY(ab->getSample(2) > 30000);
	QVERIFY(ab->getSample(3) > 30000);
	QVERIFY(ab->getSample(7) < -30000);
	QVERIFY(ab->getSample(8) < -30000);
	QVERIFY(ab->getSample(12) > 30000);
	QVERIFY(ab->getSample(13) > 30000);
	delete ab;
}

#include "test_downsamplerib.moc"


