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
  const QString& path,
  const QString& expectedTitle,
  const QString& expectedArtist,
  const QString& expectedAlbum,
  const QString& expectedComment,
  const QString& expectedGrouping,
  const QString& expectedKey
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

void testWritingToFile(const QString& path, metadata_tag_t tag) {
  Preferences prefs;
  QString initValue = "INI";
  QString noValue = GuiStrings::getInstance()->notApplicable();
  QString newValue = "ABC";
  QString empty = "";
  prefs.setMetadataWriteComment(METADATA_WRITE_NONE);
  prefs.setMetadataWriteByTagEnum(tag, METADATA_WRITE_OVERWRITE);

  MetadataWriteResult result;
  AVFileMetadataFactory factory;

  // check
  AVFileMetadata* initFileMetadata = factory.createAVFileMetadata(path);

  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    QString chk = initFileMetadata->getByTagEnum((metadata_tag_t)i);
    ASSERT_TRUE(chk == initValue || chk == noValue);
  }
  delete initFileMetadata;

  // set
  AVFileMetadata* setFileMetadata = factory.createAVFileMetadata(path);
  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++)
    result.newTags.push_back(empty);

  setFileMetadata->writeKeyByTagEnum(newValue, tag, result, prefs);
  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    if (i == tag)
      ASSERT_TRUE(result.newTags[i] == newValue);
    else
      ASSERT_TRUE(result.newTags[i] == empty);
  }
  ASSERT_TRUE(setFileMetadata->getByTagEnum(tag) == newValue);
  delete setFileMetadata;

  // reset
  AVFileMetadata* resetFileMetadata = factory.createAVFileMetadata(path);
  result.newTags.clear();
  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++)
    result.newTags.push_back(empty);

  resetFileMetadata->writeKeyByTagEnum(initValue, tag, result, prefs);
  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    if (i == tag)
      ASSERT_TRUE(result.newTags[i] == initValue);
    else
      ASSERT_TRUE(result.newTags[i] == empty);
    QString chk = resetFileMetadata->getByTagEnum((metadata_tag_t)i);
    ASSERT_TRUE(chk == initValue || chk == noValue);
  }
  delete resetFileMetadata;
}

void testId3WritingForMp3s(const QString& path, bool v1, bool v2, bool v2_3, bool v2_4) {
  AVFileMetadataFactory factory;

  // assert we have correct tags to start with
  MpegID3FileMetadata* fileMetadataPre = (MpegID3FileMetadata*)factory.createAVFileMetadata(path);
  ASSERT_EQ(fileMetadataPre->hasId3v1Tag(),   v1);
  ASSERT_EQ(fileMetadataPre->hasId3v2Tag(),   v2);
  ASSERT_EQ(fileMetadataPre->hasId3v2_3Tag(), v2_3);
  ASSERT_EQ(fileMetadataPre->hasId3v2_4Tag(), v2_4);
  delete fileMetadataPre;

  // regular test
  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    if ((metadata_tag_t)i == METADATA_TAG_GROUPING) continue;
    if ((metadata_tag_t)i == METADATA_TAG_KEY)      continue;
    testWritingToFile(path, (metadata_tag_t)i);
  }

  // and ensure we're left with the correct tags after all those changes
  MpegID3FileMetadata* fileMetadataPost = (MpegID3FileMetadata*)factory.createAVFileMetadata(path);
  ASSERT_EQ(fileMetadataPost->hasId3v1Tag(),   v1);
  ASSERT_EQ(fileMetadataPost->hasId3v2Tag(),   v2);
  ASSERT_EQ(fileMetadataPost->hasId3v2_3Tag(), v2_3);
  ASSERT_EQ(fileMetadataPost->hasId3v2_4Tag(), v2_4);
  delete fileMetadataPost;
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
    "Title", "Artist", "Album", "Comment",
    GuiStrings::getInstance()->notApplicable(),
    "Key"
  );
}

TEST (AVFileMetadataTest, ReadMp3WithNoTags) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/mp3 with no tags.mp3",
    "", "", "", "",
    GuiStrings::getInstance()->notApplicable(),
    GuiStrings::getInstance()->notApplicable()
  );
}

TEST (AVFileMetadataTest, ReadMp3WithID3v1) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/mp3 with id3 v1.mp3",
    "Title v1", "Artist v1", "Album v1", "Comment v1",
    GuiStrings::getInstance()->notApplicable(),
    GuiStrings::getInstance()->notApplicable()
  );
}

TEST (AVFileMetadataTest, ReadMp3WithID3v2_3) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/mp3 with id3 v2.3.mp3",
    "Title v2.3", "Artist v2.3", "Album v2.3",
    "Comment v2.3", "Grouping v2.3", "2.3"
  );
}

TEST (AVFileMetadataTest, ReadMp3WithID3v2_4) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/mp3 with id3 v2.4.mp3",
    "Title v2.4", "Artist v2.4", "Album v2.4",
    "Comment v2.4", "Grouping v2.4", "2.4"
  );
}

TEST (AVFileMetadataTest, ReadMp3WithID3v1Andv2_3) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/mp3 with id3 v2.3 and v1.mp3",
    "Title v2.3", "Artist v2.3", "Album v2.3",
    "Comment v2.3", "Grouping v2.3", "2.3"
  );
}

TEST (AVFileMetadataTest, ReadMp3WithID3v1Andv2_4) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/mp3 with id3 v2.4 and v1.mp3",
    "Title v2.4", "Artist v2.4", "Album v2.4",
    "Comment v2.4", "Grouping v2.4", "2.4"
  );
}

TEST (AVFileMetadataTest, ReadAiff) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/aiff.aiff",
    "Title", "Artist", "Album", "Comment", "Grouping", "Key"
  );
}

TEST (AVFileMetadataTest, ReadWav) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/wav.wav",
    "Title", "Artist", "Album", "Comment", "Grouping", "Key"
  );
}

TEST (AVFileMetadataTest, ReadAlac) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/alac.m4a",
    "Title", "Artist", "Album", "Comment", "Grouping", "Key"
  );
}

TEST (AVFileMetadataTest, ReadAac) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/aac.m4a",
    "Title", "Artist", "Album", "Comment", "Grouping", "Key"
  );
}

TEST (AVFileMetadataTest, ReadWma) {
  testPathReturnsValues(
    "../is_KeyFinder/test-resources/readTags/wma.wma",
    "Title", "Artist", "Album", "Comment", "Grouping", "Key"
  );
}

TEST (AVFileMetadataTest, WriteMissing) {

  Preferences prefs;
  AVFileMetadataFactory factory;
  AVFileMetadata* fileMetadata = factory.createAVFileMetadata("noFileHere");

  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    MetadataWriteResult result;
    for (unsigned int j = 0; j < METADATA_TAG_T_COUNT; j++)
      result.newTags.push_back("");
    fileMetadata->writeKeyByTagEnum("ABC", (metadata_tag_t)i, result, prefs);
    for (unsigned int j = 0; j < METADATA_TAG_T_COUNT; j++)
      ASSERT_TRUE(result.newTags[i] == "");
  }
}

TEST (AVFileMetadataTest, WriteNull) {
  Preferences prefs;
  AVFileMetadataFactory factory;
  AVFileMetadata* fileMetadata = factory.createAVFileMetadata("../is_KeyFinder/test-resources/notAV.pdf");

  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    MetadataWriteResult result;
    for (unsigned int j = 0; j < METADATA_TAG_T_COUNT; j++)
      result.newTags.push_back("");
    fileMetadata->writeKeyByTagEnum("ABC", (metadata_tag_t)i, result, prefs);
    for (unsigned int j = 0; j < METADATA_TAG_T_COUNT; j++)
      ASSERT_TRUE(result.newTags[i] == "");
  }
}

TEST (AVFileMetadataTest, WriteFlac) {
  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    if ((metadata_tag_t)i == METADATA_TAG_GROUPING) continue;
    testWritingToFile(
      "../is_KeyFinder/test-resources/writeTags/flac.flac",
      (metadata_tag_t)i
    );
  }
}

TEST (AVFileMetadataTest, WriteMp3WithID3v1) {
  testId3WritingForMp3s(
    "../is_KeyFinder/test-resources/writeTags/mp3 with id3 v1.mp3",
    true, false, false, false
  );
}

TEST (AVFileMetadataTest, WriteMp3WithID3v2_3) {
  testId3WritingForMp3s(
    "../is_KeyFinder/test-resources/writeTags/mp3 with id3 v2.3.mp3",
    false, true, true, false
  );
}

TEST (AVFileMetadataTest, WriteMp3WithID3v2_4) {
  testId3WritingForMp3s(
    "../is_KeyFinder/test-resources/writeTags/mp3 with id3 v2.4.mp3",
    false, true, false, true
  );
}

TEST (AVFileMetadataTest, WriteMp3WithID3v1Andv2_3) {
  testId3WritingForMp3s(
    "../is_KeyFinder/test-resources/writeTags/mp3 with id3 v2.3 and v1.mp3",
    true, true, true, false
  );
}

TEST (AVFileMetadataTest, WriteMp3WithID3v1Andv2_4) {
  testId3WritingForMp3s(
    "../is_KeyFinder/test-resources/writeTags/mp3 with id3 v2.4 and v1.mp3",
    true, true, false, true
  );
}

TEST (AVFileMetadataTest, WriteAiff) {
  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    testWritingToFile(
      "../is_KeyFinder/test-resources/writeTags/aiff.aiff",
      (metadata_tag_t)i
    );
  }
}

TEST (AVFileMetadataTest, WriteWav) {
  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    testWritingToFile(
      "../is_KeyFinder/test-resources/writeTags/wav.wav",
      (metadata_tag_t)i
    );
  }
}

TEST (AVFileMetadataTest, WriteAlac) {
  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    testWritingToFile(
      "../is_KeyFinder/test-resources/writeTags/alac.m4a",
      (metadata_tag_t)i
    );
  }
}

TEST (AVFileMetadataTest, WriteAac) {
  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    testWritingToFile(
      "../is_KeyFinder/test-resources/writeTags/aac.m4a",
      (metadata_tag_t)i
    );
  }
}

TEST (AVFileMetadataTest, WriteWma) {
  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    testWritingToFile(
      "../is_KeyFinder/test-resources/writeTags/wma.wma",
      (metadata_tag_t)i
    );
  }
}
