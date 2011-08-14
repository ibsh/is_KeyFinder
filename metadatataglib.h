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

#include "metadata.h"
#include <tag.h>
#include <taglib.h>
#include <fileref.h>
#include <apetag.h>
#include <asftag.h>
#include <id3v2tag.h>
#include <id3v2frame.h>
#include <id3v2header.h>
#include <mpegfile.h>
#include <mp4file.h>
#include <flacfile.h>
#include <tfile.h>

class TagLibMetadata : public Metadata{
public:
	TagLibMetadata(char*);
	virtual std::string getTitle() const;
	virtual std::string getArtist() const;
	virtual std::string getGrouping() const;
	virtual void setGrouping(char*);
private:
	TagLib::FileRef f;
};

#endif // METADATATAGLIB_H
