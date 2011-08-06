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
	~TagLibMetadata();
	virtual std::string getTitle() const;
	virtual std::string getArtist() const;
	virtual std::string getGrouping() const;
	virtual void setGrouping(char*);
private:
	TagLib::FileRef f;
};

#endif // METADATATAGLIB_H
