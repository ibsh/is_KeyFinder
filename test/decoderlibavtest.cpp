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

#include "decoderlibavtest.h"

TEST (AudioFileDecoderTest, MissingFile) {
    QString path("noFileHere");
    QString expectedMessage = GuiStrings::getInstance()->libavCouldNotOpenFile(-2);
    bool exceptionThrown = false;
    try {
        AudioFileDecoder d(path, 60);
    } catch (const KeyFinder::Exception& e) {
        if(QString(e.what()) == expectedMessage) exceptionThrown = true;
    }
    ASSERT_TRUE(exceptionThrown);
}

TEST (AudioFileDecoderTest, NoStreamInfo) {
    QString path("../is_KeyFinder/test-resources/notAV.pdf");
    QString expectedMessage = GuiStrings::getInstance()->libavCouldNotFindStreamInformation();
    bool exceptionThrown = false;
    try {
        AudioFileDecoder d(path, 60);
    } catch (const KeyFinder::Exception& e) {
        if(QString(e.what()) == expectedMessage) exceptionThrown = true;
    }
    ASSERT_TRUE(exceptionThrown);
}

TEST (AudioFileDecoderTest, NoAudioStream) {
    QString path("../is_KeyFinder/test-resources/noAudioStream.jpg");
    QString expectedMessage = GuiStrings::getInstance()->libavCouldNotFindAudioStream();
    bool exceptionThrown = false;
    try {
        AudioFileDecoder d(path, 60);
    } catch (const KeyFinder::Exception& e) {
        if(QString(e.what()) == expectedMessage) exceptionThrown = true;
    }
    ASSERT_TRUE(exceptionThrown);
}

TEST (AudioFileDecoderTest, OverMaxDuration) {
    QString path("../is_KeyFinder/test-resources/90secondsine.mp3");
    QString expectedMessage = GuiStrings::getInstance()->durationExceedsPreference(1, 30, 1);
    bool exceptionThrown = false;
    try {
        AudioFileDecoder d(path, 1);
    } catch (const KeyFinder::Exception& e) {
        if(QString(e.what()) == expectedMessage) exceptionThrown = true;
    }
    ASSERT_TRUE(exceptionThrown);
}
