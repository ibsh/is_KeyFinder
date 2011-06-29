#ifndef LIBAVDECODER_H
#define LIBAVDECODER_H

#include "decoder.h"
#ifndef INT64_C
#define UINT64_C(c) (c ## ULL)
#endif
extern "C"{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}

class LibAvDecoder : public AudioFileDecoder{
public:
	virtual AudioBuffer* decodeFile(char*) throw (FatalException);
private:
	int decodePacket(AVCodecContext*, AVPacket*, AudioBuffer*);
};

#endif
