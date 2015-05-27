/*************************************************************************

  Copyright 2011-2015 Ibrahim Sha'ath

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

#include "preferencestest.h"

void SettingsWrapperFake::beginGroup(const QString& g) { prefix = g; }
void SettingsWrapperFake::endGroup() { prefix.clear(); }
QVariant SettingsWrapperFake::value(const QString &key, const QVariant &defaultValue) const {
    return (hash.contains(prefix + key) ? hash[prefix + key] : defaultValue);
}
void SettingsWrapperFake::setValue(const QString &key, const QVariant &value) {
    hash[prefix + key] = value;
}
QStringList SettingsWrapperFake::allKeys() const {
    return hash.uniqueKeys();
}

TEST (PreferencesTest, ConstantsMetadataFormat) {
    ASSERT_EQ(0, (int) METADATA_FORMAT_KEYS);
    ASSERT_EQ(1, (int) METADATA_FORMAT_CUSTOM);
    ASSERT_EQ(2, (int) METADATA_FORMAT_BOTH);
}

TEST (PreferencesTest, ConstantsMetadataWrite) {
    ASSERT_EQ(0, (int) METADATA_WRITE_NONE);
    ASSERT_EQ(1, (int) METADATA_WRITE_PREPEND);
    ASSERT_EQ(2, (int) METADATA_WRITE_APPEND);
    ASSERT_EQ(3, (int) METADATA_WRITE_OVERWRITE);
}

TEST (PreferencesTest, ConstantsMetadataTag) {
    ASSERT_EQ(0, (int) METADATA_TAG_TITLE);
    ASSERT_EQ(1, (int) METADATA_TAG_ARTIST);
    ASSERT_EQ(2, (int) METADATA_TAG_ALBUM);
    ASSERT_EQ(3, (int) METADATA_TAG_COMMENT);
    ASSERT_EQ(4, (int) METADATA_TAG_GROUPING);
    ASSERT_EQ(5, (int) METADATA_TAG_KEY);
    ASSERT_EQ(6, (int) METADATA_TAG_T_COUNT);
}

TEST (PreferencesTest, ConstructorDefaultsCore) {
    SettingsWrapper* fakeSettings = new SettingsWrapperFake();
    Preferences p(fakeSettings);

    ASSERT_FLOAT_EQ(p.core.getStartingFrequencyADefault(), p.core.getStartingFrequencyA());
    ASSERT_EQ(p.core.getOctavesDefault(), p.core.getOctaves());
    ASSERT_EQ(p.core.getOffsetToCDefault(), p.core.getOffsetToC());
    ASSERT_EQ(p.core.getBandsPerOctaveDefault(), p.core.getBandsPerOctave());
    ASSERT_EQ(p.core.getTemporalWindowDefault(), p.core.getTemporalWindow());
    ASSERT_EQ(p.core.getFftFrameSizeDefault(), p.core.getFftFrameSize());
    ASSERT_EQ(p.core.getHopsPerFrameDefault(), p.core.getHopsPerFrame());
    ASSERT_FLOAT_EQ(p.core.getDirectSkStretchDefault(), p.core.getDirectSkStretch());
    ASSERT_EQ(p.core.getTuningMethodDefault(), p.core.getTuningMethod());
    ASSERT_FLOAT_EQ(p.core.getDetunedBandWeightDefault(), p.core.getDetunedBandWeight());
    ASSERT_EQ(p.core.getSegmentationDefault(), p.core.getSegmentation());
    ASSERT_EQ(p.core.getSegGaussianSizeDefault(), p.core.getSegGaussianSize());
    ASSERT_FLOAT_EQ(p.core.getSegGaussianSigmaDefault(), p.core.getSegGaussianSigma());
    ASSERT_EQ(p.core.getSegPeakPickingNeighboursDefault(), p.core.getSegPeakPickingNeighbours());
    ASSERT_EQ(p.core.getArbitrarySegmentsDefault(), p.core.getArbitrarySegments());
    ASSERT_EQ(p.core.getToneProfileDefault(), p.core.getToneProfile());
    ASSERT_EQ(p.core.getSimilarityMeasureDefault(), p.core.getSimilarityMeasure());
    std::vector<float> ctpDefault = p.core.getCustomToneProfileDefault();
    std::vector<float> ctp = p.core.getCustomToneProfile();
    ASSERT_EQ(ctpDefault.size(), ctp.size());
    for (unsigned int i = 0; i < ctp.size(); i++)
        ASSERT_FLOAT_EQ(ctpDefault[i], ctp[i]);
}

TEST (PreferencesTest, ConstructorDefaultsGui) {
    SettingsWrapper* fakeSettings = new SettingsWrapperFake();
    Preferences p(fakeSettings);

    ASSERT_FALSE(p.getWriteToFilesAutomatically());
    ASSERT_TRUE(p.getParallelBatchJobs());
    ASSERT_FALSE(p.getApplyFileExtensionFilter());
    ASSERT_EQ(METADATA_WRITE_NONE, p.getMetadataWriteTitle());
    ASSERT_EQ(METADATA_WRITE_NONE, p.getMetadataWriteArtist());
    ASSERT_EQ(METADATA_WRITE_NONE, p.getMetadataWriteAlbum());
    ASSERT_EQ(METADATA_WRITE_PREPEND, p.getMetadataWriteComment());
    ASSERT_EQ(METADATA_WRITE_NONE, p.getMetadataWriteGrouping());
    ASSERT_EQ(METADATA_WRITE_NONE, p.getMetadataWriteKey());
    ASSERT_EQ(METADATA_WRITE_NONE, p.getMetadataWriteFilename());
    ASSERT_EQ(METADATA_FORMAT_KEYS, p.getMetadataFormat());
    ASSERT_FALSE(p.getSkipFilesWithExistingTags());
    ASSERT_EQ(60, p.getMaxDuration());
#ifdef Q_OS_WIN
    QString iTunesLibraryPathDefault = QDir::homePath() + "/My Music/iTunes/iTunes Music Library.xml";
    QString traktorLibraryPathDefault = QDir::homePath() + "/My Documents/Native Instruments/Traktor 2.1.2/collection.nml";
    QString seratoLibraryPathDefault = QDir::homePath() + "/My Music/_Serato_/database V2";
#else
    QString iTunesLibraryPathDefault = QDir::homePath() + "/Music/iTunes/iTunes Music Library.xml";
    QString traktorLibraryPathDefault = QDir::homePath() + "/Documents/Native Instruments/Traktor 2.1.2/collection.nml";
    QString seratoLibraryPathDefault = QDir::homePath() + "/Music/_Serato_/database V2";
#endif
    ASSERT_EQ(iTunesLibraryPathDefault, p.getITunesLibraryPath());
    ASSERT_EQ(traktorLibraryPathDefault, p.getTraktorLibraryPath());
    ASSERT_EQ(seratoLibraryPathDefault, p.getSeratoLibraryPath());
    QStringList customKeyCodes = p.getCustomKeyCodes();
    ASSERT_EQ(25, customKeyCodes.length());
    for (int i = 0; i < customKeyCodes.length(); i++)
        ASSERT_EQ(QString(), customKeyCodes[i]);
    QStringList fileExtensions = p.getFilterFileExtensions();
    ASSERT_EQ(8, fileExtensions.length());
    ASSERT_EQ("mp3", fileExtensions[0]);
    ASSERT_EQ("m4a", fileExtensions[1]);
    ASSERT_EQ("mp4", fileExtensions[2]);
    ASSERT_EQ("wma", fileExtensions[3]);
    ASSERT_EQ("flac", fileExtensions[4]);
    ASSERT_EQ("aif", fileExtensions[5]);
    ASSERT_EQ("aiff", fileExtensions[6]);
    ASSERT_EQ("wav", fileExtensions[7]);
    ASSERT_EQ(QByteArray(), p.getBatchWindowState());
    ASSERT_EQ(QByteArray(), p.getBatchWindowGeometry());
    ASSERT_EQ(QByteArray(), p.getBatchWindowSplitterState());
}

TEST (PreferencesTest, QSettingsWrite) {
    SettingsWrapper* fakeSettings = new SettingsWrapperFake();
    ASSERT_FLOAT_EQ(0, fakeSettings->value("corestartingFrequencyA", 0).toFloat());
    Preferences writer(fakeSettings);
    writer.save();
    ASSERT_FLOAT_EQ(27.5, fakeSettings->value("corestartingFrequencyA", 0).toFloat());
    writer.core.setStartingFrequencyA(55.0);
    writer.save();
    ASSERT_FLOAT_EQ(55.0, fakeSettings->value("corestartingFrequencyA", 0).toFloat());
}

TEST (PreferencesTest, QSettingsRead) {
    SettingsWrapper* fakeSettings = new SettingsWrapperFake();
    fakeSettings->beginGroup("core");
    fakeSettings->setValue("startingFrequencyA", (float)110.0);
    fakeSettings->endGroup();
    Preferences reader(fakeSettings);
    ASSERT_FLOAT_EQ(110.0, reader.core.getStartingFrequencyA());
}

TEST (PreferencesTest, Equality) {
    Preferences a;
    Preferences b;
    ASSERT_TRUE(a.equivalentTo(b));
    a.core.setStartingFrequencyA(55.0);
    ASSERT_FALSE(a.equivalentTo(b));
    a.core.setStartingFrequencyA(27.5);
    ASSERT_TRUE(a.equivalentTo(b));
    a.setMaxDuration(61);
    ASSERT_FALSE(a.equivalentTo(b));
}

TEST (PreferencesTest, NewStringDeterminesNoWriteCorrectly) {
    Preferences prefs;
    QString currentData = "data";
    QString newData = "key";
    metadata_write_t write = METADATA_WRITE_NONE;
    QString expectedOutput= "";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesOverwriteCorrectly) {
    Preferences prefs;
    QString currentData = "data";
    QString newData = "key";
    metadata_write_t write = METADATA_WRITE_OVERWRITE;
    QString expectedOutput= "key";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesOverwriteToSameCorrectly) {
    Preferences prefs;
    QString currentData = "key";
    QString newData = "key";
    metadata_write_t write = METADATA_WRITE_OVERWRITE;
    QString expectedOutput= "";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesPrependCorrectly) {
    Preferences prefs;
    prefs.setMetadataDelimiter("_");
    QString currentData = "data";
    QString newData = "key";
    metadata_write_t write = METADATA_WRITE_PREPEND;
    QString expectedOutput= "key_data";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesPrependToSameCorrectly) {
    Preferences prefs;
    QString currentData = "key";
    QString newData = "key";
    metadata_write_t write = METADATA_WRITE_PREPEND;
    QString expectedOutput= "";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesPrependToEmptyCorrectly) {
    Preferences prefs;
    prefs.setMetadataDelimiter("_");
    QString currentData = "";
    QString newData = "key";
    metadata_write_t write = METADATA_WRITE_PREPEND;
    QString expectedOutput= "key";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesPrependToDifferentDelimiterCorrectly) {
    Preferences prefs;
    prefs.setMetadataDelimiter("_");
    QString currentData = "key - data";
    QString newData = "key";
    metadata_write_t write = METADATA_WRITE_PREPEND;
    QString expectedOutput= "";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesPrependToUnexpectedDelimiterCorrectly) {
    Preferences prefs;
    QString currentData = "key.data";
    QString newData = "key";
    metadata_write_t write = METADATA_WRITE_PREPEND;
    QString expectedOutput= "";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesPrependToSimilarDataCorrectly) {
    Preferences prefs;
    prefs.setMetadataDelimiter("_");
    QString currentData = "Amazon";
    QString newData = "Am";
    metadata_write_t write = METADATA_WRITE_PREPEND;
    QString expectedOutput= "Am_Amazon";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesAppendCorrectly) {
    Preferences prefs;
    prefs.setMetadataDelimiter("_");
    QString currentData = "data";
    QString newData = "key";
    metadata_write_t write = METADATA_WRITE_APPEND;
    QString expectedOutput= "data_key";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesAppendToSameCorrectly) {
    Preferences prefs;
    QString currentData = "key";
    QString newData = "key";
    metadata_write_t write = METADATA_WRITE_APPEND;
    QString expectedOutput= "";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesAppendToEmptyCorrectly) {
    Preferences prefs;
    prefs.setMetadataDelimiter("_");
    QString currentData = "";
    QString newData = "key";
    metadata_write_t write = METADATA_WRITE_APPEND;
    QString expectedOutput= "key";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesAppendToDifferentDelimiterCorrectly) {
    Preferences prefs;
    prefs.setMetadataDelimiter("_");
    QString currentData = "data - key";
    QString newData = "key";
    metadata_write_t write = METADATA_WRITE_APPEND;
    QString expectedOutput= "";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesAppendToUnexpectedDelimiterCorrectly) {
    Preferences prefs;
    QString currentData = "data.key";
    QString newData = "key";
    metadata_write_t write = METADATA_WRITE_APPEND;
    QString expectedOutput= "";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesAppendToSimilarDataCorrectly) {
    Preferences prefs;
    prefs.setMetadataDelimiter("_");
    QString currentData = "WOOHAA";
    QString newData = "A";
    metadata_write_t write = METADATA_WRITE_APPEND;
    QString expectedOutput= "WOOHAA_A";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesOverwriteCorrectlyForAllOptions) {
    Preferences prefs;
    QString currentData = "Bbm";
    QString newData = "";
    metadata_write_t write = METADATA_WRITE_OVERWRITE;
    QString expectedOutput= "";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesPrependCorrectlyForAllOptions) {
    Preferences prefs;
    QString currentData = "Em - data";
    QString newData = "";
    metadata_write_t write = METADATA_WRITE_PREPEND;
    QString expectedOutput= "";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDeterminesAppendCorrectlyForAllOptions) {
    Preferences prefs;
    QString currentData = "data - D";
    QString newData = "";
    metadata_write_t write = METADATA_WRITE_APPEND;
    QString expectedOutput= "";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, METADATA_CHARLIMIT_OTHERS, write));
}

TEST (PreferencesTest, NewStringDealsWithCharLimitsCorrectly) {
    Preferences prefs;
    QStringList camelot;
    camelot << "XXX and a bunch of other data";
    for (unsigned int i = 1; i <26; i++)
        camelot << "";
    prefs.setCustomKeyCodes(camelot);
    prefs.setMetadataFormat(METADATA_FORMAT_CUSTOM);
    QString currentData = "XXX";
    QString newData = "";
    metadata_write_t write = METADATA_WRITE_OVERWRITE;
    QString expectedOutput= "";
    ASSERT_EQ(expectedOutput, prefs.newString(newData, currentData, 3, write));
}
