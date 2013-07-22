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

#ifndef METADATATAGLIB_H
#define METADATATAGLIB_H

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

#ifdef Q_OS_WIN
#include "os_windows.h"
#endif

#include "preferences.h"
#include "metadatawriteresult.h"

class TagLibMetadata{
public:
  TagLibMetadata(const QString&);
  ~TagLibMetadata();
  QString getByTagEnum(metadata_tag_t) const;
  QString getTitle() const;
  QString getArtist() const;
  QString getAlbum() const;
  QString getComment() const;
  QString getGrouping() const;
  QString getKey() const;
  MetadataWriteResult writeKeyToMetadata(KeyFinder::key_t, const Preferences&);
protected:
  TagLib::File* f;
  bool setByTagEnum(const QString&, metadata_tag_t);
  void writeKeyByTagEnum(const QString&, metadata_tag_t, MetadataWriteResult&, const Preferences&);
  bool setTitle(const QString&);
  bool setArtist(const QString&);
  bool setAlbum(const QString&);
  bool setComment(const QString&);
  bool setGrouping(const QString&);
  bool setKey(const QString&);
  QString getGroupingId3(const TagLib::ID3v2::Tag*) const;
  QString getKeyId3(const TagLib::ID3v2::Tag*) const;
  void setITunesCommentId3v2(TagLib::ID3v2::Tag*, const QString&);
  bool setGroupingId3(TagLib::ID3v2::Tag*, const QString&);
  bool setKeyId3(TagLib::ID3v2::Tag*, const QString&);
};

#endif // METADATATAGLIB_H
