#ifndef AUDIOFILEDECODER_H
#define AUDIOFILEDECODER_H

#include "exception.h"
#include "audiobuffer.h"

#include <string>

class AudioFileDecoder{
public:
	virtual AudioBuffer* decodeFile(char*) throw (Exception) = 0;
	static AudioFileDecoder* getDecoder(const std::string&);
};

#endif
