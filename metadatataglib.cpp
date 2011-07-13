#include "metadatataglib.h"

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
	// Beware; setGrouping is not usually a member of the TagLib API;
	// it only exists in my custom version of the v1.7 source.
	f.tag()->setGrouping(c);
	f.save();
}
