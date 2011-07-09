#ifndef LIBSNDFILEDECODER_H
#define LIBSNDFILEDECODER_H

#include "decoder.h"

class LibSndFileDecoder : public AudioFileDecoder{
public:
	virtual AudioBuffer* decodeFile(char*) throw (Exception);
};

#endif
