#include <QtCore/QString>
#include <QtTest/QtTest>
#include "unittest.h"

#include "chromagram.h"
#include "preferences.h"

class Chromagram_Test : public QObject{
	Q_OBJECT
public:
	Chromagram_Test();
private Q_SLOTS:
	void testConstructor();
	void testSetMagnitude();
	void testDecomposition();
};
DECLARE_TEST(Chromagram_Test)

Chromagram_Test::Chromagram_Test(){}

void Chromagram_Test::testConstructor(){
	Chromagram* ch = new Chromagram(1,1);
	QVERIFY(ch->getBins() == 1);
	QVERIFY(ch->getHops() == 1);
	QVERIFY(ch->getMagnitude(0,0) == 0.0);
	delete ch;
}

void Chromagram_Test::testSetMagnitude(){
	Chromagram* ch = new Chromagram(1,1);
	ch->setMagnitude(0,0,5.0);
	QVERIFY(ch->getMagnitude(0,0) == 5.0);
	delete ch;
}

void Chromagram_Test::testDecomposition(){
	int octaves = 6;
	int semitones = 12;
	int bandsPerSemitone = 3;
	Preferences prefs;
	prefs.setBandsPerSemitone(bandsPerSemitone);
	float weight = prefs.getDetunedBandWeight();
	Chromagram* ch = new Chromagram(1,octaves*semitones*bandsPerSemitone);
	QVERIFY(ch->getBins() == 216);
	// first semitone of first octave, tuned down
	ch->setMagnitude(0,0,10.0);
	ch->setMagnitude(0,1,1.0);
	ch->setMagnitude(0,2,1.0);
	// first semitone of second octave, tuned to concert pitch
	ch->setMagnitude(0,36,1.0);
	ch->setMagnitude(0,37,10.0);
	ch->setMagnitude(0,38,1.0);
	// first semitone of second octave, tuned up
	ch->setMagnitude(0,72,1.0);
	ch->setMagnitude(0,73,1.0);
	ch->setMagnitude(0,74,10.0);
	// decompose to 1 band per semitone
	ch->decomposeToTwelveBpo(prefs);
	QVERIFY(ch->getBins() == 72);
	// adaptive tuning should mean all three set semitones come out the same
	QVERIFY(ch->getMagnitude(0,0) == 10 + 2 * 1.0 * weight);
	QVERIFY(ch->getMagnitude(0,12) == 10 + 2 * 1.0 * weight);
	QVERIFY(ch->getMagnitude(0,24) == 10 + 2 * 1.0 * weight);
	QVERIFY(ch->getMagnitude(0,36) == 0.0);
	// decompose to one octave
	ch->decomposeToOneOctave(prefs);
	QVERIFY(ch->getBins() == 12);
	// test magnitude is now halved, since only three of six were set
	QVERIFY(ch->getMagnitude(0,0) == 0.5*(10 + 2 * 1.0 * weight));
	delete ch;
}

#include "test_chromagram.moc"

