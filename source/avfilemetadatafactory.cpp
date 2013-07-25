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

#include "avfilemetadatafactory.h"

QMutex factory_mutex; // global mutex on file resolution

AVFileMetadata* AVFileMetadataFactory::createAVFileMetadata(const QString& filePath) const {
  QMutexLocker locker(&factory_mutex); // mutex the constructor

  #ifdef Q_OS_WIN
    // Using utf16_to_utf8 here, as per decoderlibav, leads to a null file reference.
    const wchar_t* filePathCh = reinterpret_cast<const wchar_t*>(filePath.constData());
  #else
    QByteArray encodedPath = QFile::encodeName(filePath);
    const char* filePathCh = encodedPath;
  #endif

  TagLib::FileRef* fr;
  TagLib::File* f = NULL;

  fr = new TagLib::FileRef(filePathCh);
  if (!fr->isNull()) f = fr->file();

  if (f == NULL || !f->isValid()) {
    delete fr;
    #ifdef Q_OS_WIN
      qWarning("TagLib returned NULL File for %s", utf16_to_utf8(filePathCh));
    #else
      qWarning("TagLib returned NULL File for %s", filePathCh);
    #endif
    return new NullFileMetadata(NULL, NULL);
  }

  locker.unlock();

  TagLib::FLAC::File* fileTestFlac = dynamic_cast<TagLib::FLAC::File*>(f);
  if (fileTestFlac != NULL) return new FlacFileMetadata(fr, f, fileTestFlac);

  TagLib::MPEG::File* fileTestMpeg = dynamic_cast<TagLib::MPEG::File*>(f);
  if (fileTestMpeg != NULL) return new MpegID3FileMetadata(fr, f, fileTestMpeg);

  TagLib::RIFF::AIFF::File* fileTestAiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(f);
  if (fileTestAiff != NULL) return new AiffID3FileMetadata(fr, f, fileTestAiff);

  TagLib::RIFF::WAV::File* fileTestWav = dynamic_cast<TagLib::RIFF::WAV::File*>(f);
  if (fileTestWav != NULL) return new WavID3FileMetadata(fr, f, fileTestWav);

  TagLib::MP4::File* fileTestMp4 = dynamic_cast<TagLib::MP4::File*>(f);
  if (fileTestMp4 != NULL) return new Mp4FileMetadata(fr, f, fileTestMp4);

  TagLib::ASF::File* fileTestAsf = dynamic_cast<TagLib::ASF::File*>(f);
  if (fileTestAsf != NULL) return new AsfFileMetadata(fr, f, fileTestAsf);

  TagLib::APE::File* fileTestApe = dynamic_cast<TagLib::APE::File*>(f);
  if (fileTestApe != NULL) return new ApeFileMetadata(fr, f, fileTestApe);

  return new AVFileMetadata(fr, f);
}
