#ifndef LIBAVDECODER_H
#define LIBAVDECODER_H

#include "decoder.h"
#ifndef INT64_C
#define UINT64_C(c) (c ## ULL)
#endif
#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096
extern "C"{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}

class LibAvDecoder : public AudioFileDecoder{
public:
	virtual AudioBuffer* decodeFile(char*) throw (Exception);
private:
	int decodePacket(AVCodecContext*, AVPacket*, AudioBuffer*);
};

#endif
