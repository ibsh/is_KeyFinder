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

#ifndef METADATATAGLIB_H
#define METADATATAGLIB_H

#include <QString>
#include <QFile>
#include <QDebug>
#include <QMutex>

#include <tag.h>
#include <taglib.h>
#include <fileref.h>
#include <tfile.h>
#include <id3v1tag.h>
#include <id3v2tag.h>
#include <textidentificationframe.h>
#include <asftag.h>
#include <apetag.h>
#include <mp4tag.h>
// #include <apefile.h> // removed with Windows version; difficulty in compilation
#include <asffile.h>
#include <flacfile.h>
#include <mp4file.h>
#include <mpcfile.h>
#include <mpegfile.h>
#include <oggfile.h>
#include <oggflacfile.h>
#include <speexfile.h>
#include <vorbisfile.h>
#include <rifffile.h>
#include <aifffile.h>
#include <wavfile.h>
#include <trueaudiofile.h>
#include <wavpackfile.h>

#include "preferences.h"

class TagLibMetadata{
public:
  TagLibMetadata(const QString&);
	~TagLibMetadata();
	QString getTitle() const;
	QString getArtist() const;
  QString getComment() const;
	QString getGrouping() const;
  QString getKey() const;
  QString writeKeyToMetadata(int, const Preferences&);
private:
  TagLib::File* f;
  int setComment(const QString&);
  int setGrouping(const QString&);
  int setKey(const QString&);
};

#endif // METADATATAGLIB_H
