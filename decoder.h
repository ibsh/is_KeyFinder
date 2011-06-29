#ifndef AUDIOFILEDECODER_H
#define AUDIOFILEDECODER_H

#include "fatalexception.h"
#include "audiobuffer.h"

class AudioFileDecoder{
public:
	virtual AudioBuffer* decodeFile(char*) throw (FatalException) = 0;
};

#endif
