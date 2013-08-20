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

#include "avfilemetadatatest.h"

void testPathReturnsValues(
  QString path,
  QString expectedTitle,
  QString expectedArtist,
  QString expectedAlbum,
  QString expectedComment,
  QString expectedGrouping,
  QString expectedKey
) {

  AVFileMetadataFactory factory;
  AVFileMetadata* fileMetadata = factory.createAVFileMetadata(path);

  ASSERT_TRUE(fileMetadata->getTitle() == expectedTitle);
  ASSERT_TRUE(fileMetadata->getArtist() == expectedArtist);
  ASSERT_TRUE(fileMetadata->getAlbum() == expectedAlbum);
  ASSERT_TRUE(fileMetadata->getComment() == expectedComment);
  ASSERT_TRUE(fileMetadata->getGrouping() == expectedGrouping);
  ASSERT_TRUE(fileMetadata->getKey() == expectedKey);
}

TEST (AVFileMetadataTest, ReadMissing) {
  testPathReturnsValues(
    "noFileHere",
    GuiStrings::getInstance()->notApplicable(),
    GuiStrings::getInstance()->notApplicable(),
    GuiStrings::getInstance()->notApplicable(),
    GuiStrings::getInstance()->notApplicable(),
    GuiStrings::getInstance()->notApplicable(),
    GuiStrings::getInstance()->notApplicable()
  );
}

TEST (AVFileMetadataTest, ReadNull) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/notAV.pdf",
    GuiStrings::getInstance()->notApplicable(),
    GuiStrings::getInstance()->notApplicable(),
    GuiStrings::getInstance()->notApplicable(),
    GuiStrings::getInstance()->notApplicable(),
    GuiStrings::getInstance()->notApplicable(),
    GuiStrings::getInstance()->notApplicable()
  );
}

TEST (AVFileMetadataTest, ReadFlac) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/flac.flac",
    "Title",
    "Artist",
    "Album",
    "Comment",
    GuiStrings::getInstance()->notApplicable(),
    "Key"
  );
}

TEST (AVFileMetadataTest, ReadMp3WithNoTags) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/mp3 with no tags.mp3",
    "",
    "",
    "",
    "",
    GuiStrings::getInstance()->notApplicable(),
    GuiStrings::getInstance()->notApplicable()
  );
}

TEST (AVFileMetadataTest, ReadMp3WithID3v1) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/mp3 with id3 v1.mp3",
    "Title v1",
    "Artist v1",
    "Album v1",
    "Comment v1",
    GuiStrings::getInstance()->notApplicable(),
    GuiStrings::getInstance()->notApplicable()
  );
}

TEST (AVFileMetadataTest, ReadMp3WithID3v1Andv2_3) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/mp3 with id3 v2.3 and v1.mp3",
    "Title v2.3",
    "Artist v2.3",
    "Album v2.3",
    "Comment v2.3",
    "Grouping v2.3",
    "2.3"
  );
}

TEST (AVFileMetadataTest, ReadMp3WithID3v1Andv2_4) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/mp3 with id3 v2.4 and v1.mp3",
    "Title v2.4",
    "Artist v2.4",
    "Album v2.4",
    "Comment v2.4",
    "Grouping v2.4",
    "2.4"
  );
}

TEST (AVFileMetadataTest, ReadAiff) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/aiff.aiff",
    "Title",
    "Artist",
    "Album",
    "Comment",
    "Grouping",
    "Key"
  );
}

TEST (AVFileMetadataTest, ReadWav) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/wav.wav",
    "Title",
    "Artist",
    "Album",
    "Comment",
    "Grouping",
    "Key"
  );
}

TEST (AVFileMetadataTest, ReadAlac) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/alac.m4a",
    "Title",
    "Artist",
    "Album",
    "Comment",
    "Grouping",
    "Key"
  );
}

TEST (AVFileMetadataTest, ReadAac) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/aac.m4a",
    "Title",
    "Artist",
    "Album",
    "Comment",
    "Grouping",
    "Key"
  );
}

TEST (AVFileMetadataTest, ReadWma) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/wma.wma",
    "Title",
    "Artist",
    "Album",
    "Comment",
    "Grouping",
    "Key"
  );
}
