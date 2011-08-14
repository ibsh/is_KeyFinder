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

#include "metadatataglib.h"

TagLibMetadata::TagLibMetadata(char* fileName){
	f = TagLib::FileRef(fileName);
}

std::string TagLibMetadata::getTitle() const{
	if(f.isNull())
		return "";
	TagLib::String out = f.tag()->title();
	return (std::string)out.toCString();
}

std::string TagLibMetadata::getArtist() const{
	if(f.isNull())
		return "";
	TagLib::String out = f.tag()->artist();
	return (std::string)out.toCString();
}

std::string TagLibMetadata::getGrouping() const{
	if(f.isNull())
		return "";
	TagLib::String out = f.tag()->grouping();
	return (std::string)out.toCString();
}

void TagLibMetadata::setGrouping(char* c){
	// Beware; setGrouping is not usually a member of the TagLib API;
	// it only exists in my custom version of the v1.7 source.
	f.tag()->setGrouping(c);
	f.save();
}
