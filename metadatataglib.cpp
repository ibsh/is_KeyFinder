#include "metadatataglib.h"

/*
Available in TagLib without much effort:
Album
Artist
Comment
Genre
Title
Track
Year
*/

TagLibMetadata::TagLibMetadata(char* fileName){
	f = TagLib::FileRef(fileName);
}

TagLibMetadata::~TagLibMetadata(){
}

std::string TagLibMetadata::getTitle() const{
	TagLib::String out = f.tag()->title();
	return (std::string)out.toCString();
}

std::string TagLibMetadata::getArtist() const{
	TagLib::String out = f.tag()->artist();
	return (std::string)out.toCString();
}

std::string TagLibMetadata::getGrouping() const{
	TagLib::String out = f.tag()->grouping();
	return (std::string)out.toCString();
}

void TagLibMetadata::setGrouping(char* c){
	f.tag()->setGrouping(c);
	f.save();
}
