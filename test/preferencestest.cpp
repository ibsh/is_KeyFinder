/*************************************************************************

  Copyright 2011-2013 Ibrahim Sha'ath

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
  ASSERT_EQ(6, METADATA_TAG_T_COUNT);
}

class QSettingsFake : public QSettings {
public:
  QSettingsFake() : QSettings("fake", QSettings::IniFormat, NULL) { }
  void beginGroup(const QString& p) { prefix = p; }
  void endGroup() { prefix.clear(); }
  QVariant value(const QString &key, const QVariant& /* defaultValue */) const {
    return hash[prefix + key];
  }
  void setValue(const QString& key, const QVariant& value) {
    hash[prefix + key] = value;
  }
  QStringList allKeys() const {
    return hash.keys();
  }
  QHash<QString, QVariant> hash;
  QString prefix;
};

TEST (PreferencesTest, ConstructorDefaults) {
  QSettingsFake* fakeSettings = new QSettingsFake();
  Preferences p((QSettings*)fakeSettings);

  // core: defaults from LibKeyFinder
  KeyFinder::Parameters core;
  ASSERT_FLOAT_EQ(core.getStartingFreqADefault(), p.core.getStartingFreqA());
  ASSERT_EQ(core.getOctavesDefault(), p.core.getOctaves());
  ASSERT_EQ(core.getOffsetToCDefault(), p.core.getOffsetToC());
  ASSERT_EQ(core.getBandsPerOctaveDefault(), p.core.getBandsPerOctave());
  ASSERT_EQ(core.getTemporalWindowDefault(), p.core.getTemporalWindow());
  ASSERT_EQ(core.getFftFrameSizeDefault(), p.core.getFftFrameSize());
  ASSERT_EQ(core.getHopsPerFrameDefault(), p.core.getHopsPerFrame());
  ASSERT_FLOAT_EQ(core.getDirectSkStretchDefault(), p.core.getDirectSkStretch());
  // TODO: finish me.
}
