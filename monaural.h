#ifndef MONAURAL_H
#define MONAURAL_H

#include "exception.h"
#include "audiobuffer.h"

class Monaural{
public:
		virtual AudioBuffer* makeMono(AudioBuffer*);
};

#endif
