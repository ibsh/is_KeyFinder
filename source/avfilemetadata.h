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

#ifndef AVFILEMETADATA_H
#define AVFILEMETADATA_H

#include <QString>
#include <QFile>
#include <QDebug>
#include <QMutex>

#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include <taglib/tfile.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/textidentificationframe.h>
#include <taglib/commentsframe.h>
#include <taglib/asftag.h>
#include <taglib/apetag.h>
#include <taglib/mp4tag.h>
#include <taglib/apefile.h>
#include <taglib/asffile.h>
#include <taglib/flacfile.h>
#include <taglib/mp4file.h>
#include <taglib/mpcfile.h>
#include <taglib/mpegfile.h>
#include <taglib/oggfile.h>
#include <taglib/oggflacfile.h>
#include <taglib/speexfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/rifffile.h>
#include <taglib/aifffile.h>
#include <taglib/wavfile.h>
#include <taglib/trueaudiofile.h>
#include <taglib/wavpackfile.h>

#include "preferences.h"
#include "metadatawriteresult.h"
#ifdef Q_OS_WIN
#include "os_windows.h"
#endif

// for "generic" files without any special treatment
class AVFileMetadata {
public:
  AVFileMetadata(TagLib::FileRef* fr, TagLib::File* f);
  virtual ~AVFileMetadata();
  virtual QString getByTagEnum(metadata_tag_t) const;
  virtual QString getTitle() const;
  virtual QString getArtist() const;
  virtual QString getAlbum() const;
  virtual QString getComment() const;
  virtual QString getGrouping() const;
  virtual QString getKey() const;
  virtual MetadataWriteResult writeKeyToMetadata(KeyFinder::key_t, const Preferences&);
protected:
  TagLib::FileRef* fr;
  TagLib::File* genericFile;
  virtual bool setByTagEnum(const QString&, metadata_tag_t);
  virtual void writeKeyByTagEnum(const QString&, metadata_tag_t, MetadataWriteResult&, const Preferences&);
  virtual bool setTitle(const QString&);
  virtual bool setArtist(const QString&);
  virtual bool setAlbum(const QString&);
  virtual bool setComment(const QString&);
  virtual bool setGrouping(const QString&);
  virtual bool setKey(const QString&);
};

class NullFileMetadata : public AVFileMetadata {
public:
  NullFileMetadata(TagLib::FileRef* fr, TagLib::File* f);
  virtual ~NullFileMetadata();
  virtual QString getByTagEnum(metadata_tag_t) const;
  virtual QString getTitle() const;
  virtual QString getArtist() const;
  virtual QString getAlbum() const;
  virtual QString getComment() const;
protected:
  virtual bool setTitle(const QString&);
  virtual bool setArtist(const QString&);
  virtual bool setAlbum(const QString&);
  virtual bool setComment(const QString&);
};

class FlacFileMetadata : public AVFileMetadata {
public:
  FlacFileMetadata(TagLib::FileRef* fr, TagLib::File* g, TagLib::FLAC::File* s);
  virtual QString getComment() const;
  virtual QString getKey() const;
protected:
  TagLib::FLAC::File* specificFile;
  virtual bool setComment(const QString&);
  virtual bool setKey(const QString&);
};

class MpegID3FileMetadata : public AVFileMetadata {
public:
  MpegID3FileMetadata(TagLib::FileRef* fr, TagLib::File* g, TagLib::MPEG::File* s);
  virtual QString getGrouping() const;
  virtual QString getKey() const;
protected:
  TagLib::MPEG::File* specificFile;
  virtual bool setComment(const QString&);
  virtual bool setGrouping(const QString&);
  virtual bool setKey(const QString&);
  QString getGroupingId3(const TagLib::ID3v2::Tag* tag, bool v1check) const;
  QString getKeyId3(const TagLib::ID3v2::Tag* tag, bool v1check) const;
  void setITunesCommentId3(TagLib::ID3v2::Tag* tag, const QString& cmt, bool v1check);
  bool setGroupingId3(TagLib::ID3v2::Tag* tag, const QString& grp, bool v1check);
  bool setKeyId3(TagLib::ID3v2::Tag* tag, const QString& key, bool v1check);
};

class AiffID3FileMetadata : public MpegID3FileMetadata {
public:
  AiffID3FileMetadata(TagLib::FileRef* fr, TagLib::File* g, TagLib::RIFF::AIFF::File* s);
  virtual QString getGrouping() const;
  virtual QString getKey() const;
protected:
  TagLib::RIFF::AIFF::File* specificFile;
  virtual bool setComment(const QString&);
  virtual bool setGrouping(const QString&);
  virtual bool setKey(const QString&);
};

class WavID3FileMetadata : public AiffID3FileMetadata {
public:
  WavID3FileMetadata(TagLib::FileRef* fr, TagLib::File* g, TagLib::RIFF::WAV::File* s);
  virtual QString getGrouping() const;
  virtual QString getKey() const;
protected:
  TagLib::RIFF::WAV::File* specificFile;
  virtual bool setComment(const QString&);
  virtual bool setGrouping(const QString&);
  virtual bool setKey(const QString&);
};

class Mp4FileMetadata : public AVFileMetadata {
public:
  Mp4FileMetadata(TagLib::FileRef* fr, TagLib::File* g, TagLib::MP4::File* s);
  virtual QString getGrouping() const;
  virtual QString getKey() const;
protected:
  TagLib::MP4::File* specificFile;
  virtual bool setGrouping(const QString&);
  virtual bool setKey(const QString&);
};

class AsfFileMetadata : public AVFileMetadata {
public:
  AsfFileMetadata(TagLib::FileRef* fr, TagLib::File* g, TagLib::ASF::File* s);
  virtual QString getGrouping() const;
  virtual QString getKey() const;
protected:
  TagLib::ASF::File* specificFile;
  virtual bool setGrouping(const QString&);
  virtual bool setKey(const QString&);
};

class ApeFileMetadata : public AVFileMetadata {
public:
  ApeFileMetadata(TagLib::FileRef* fr, TagLib::File* g, TagLib::APE::File* s);
  virtual QString getGrouping() const;
protected:
  TagLib::APE::File* specificFile;
  virtual bool setGrouping(const QString&);
};

#endif // AVFILEMETADATA_H
