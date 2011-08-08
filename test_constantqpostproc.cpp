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
#include "fftwanalyser.h"
#include "constantqpostproc.h"
#include "chromagram.h"

class ConstantQPostProc_Test : public QObject{
	Q_OBJECT
public:
	ConstantQPostProc_Test();
private Q_SLOTS:
	void testTransform();
};
DECLARE_TEST(ConstantQPostProc_Test)

ConstantQPostProc_Test::ConstantQPostProc_Test(){}

void ConstantQPostProc_Test::testTransform(){
	LibAvDecoder* dec = new LibAvDecoder;
	AudioBuffer* ab = NULL;
	try{
		ab = dec->decodeFile("../is_KeyFinder/_testaudio/audio-sine-chord.wav");
	}catch(...){
		QFAIL("Decoder failed");
	}
	delete dec;
	Preferences prefs;
	prefs.setBandsPerSemitone(1);
	prefs.setDownsampleFactor(1);
	prefs.setFftPostProcessor('c');
	SpectrumAnalyser* sa = new FftwAnalyser(44100,prefs);
	Chromagram* ch = sa->chromagram(ab);
	delete sa;
	delete ab;
	/*
		audio-sine-chord.wav contains an A minor seventh chord starting at A440,
		so we test that those four notes are roughly equal in energy, and that the
		other notes are below some threshold.
	*/
	int notes[] = {48,51,55,58}; // A4, C5, E5, G5.
	// normalise
	float norm = ch->getMagnitude(0,notes[0]);
	for(int i=0; i<ch->getBins(); i++)
		if(i != 48)
			ch->setMagnitude(0,i,ch->getMagnitude(0,i) / norm);
	ch->setMagnitude(0,48,1.0);
	// test all bins
	for(int i=0; i<ch->getBins(); i++){
		bool tested = false;
		for(int j=0; j<4; j++){
			if(i == notes[j]){
				QVERIFY(ch->getMagnitude(0,i) > 0.99);
				QVERIFY(ch->getMagnitude(0,i) < 1.01);
				tested = true;
				break;
			}
		}
		if(!tested){
			QVERIFY(ch->getMagnitude(0,i) < 0.0001);
		}
	}
	delete ch;
}

#include "test_constantqpostproc.moc"

