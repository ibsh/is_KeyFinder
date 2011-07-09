#ifndef METADATAREADERLIBAV_H
#define METADATAREADERLIBAV_H

#include "metadatareader.h"
#ifndef INT64_C
#define UINT64_C(c) (c ## ULL)
#endif
extern "C"{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/dict.h>
}

class LibAvMetadataReader : public MetadataReader{
public:
	LibAvMetadataReader(char*);
	~LibAvMetadataReader();
	virtual std::string getTitle();
	virtual std::string getArtist();
private:
	AVFormatContext* fCtx;
};

#endif // METADATAREADERLIBAV_H
