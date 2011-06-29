#ifndef MONAURAL_H
#define MONAURAL_H

#include "fatalexception.h"
#include "audiobuffer.h"

class Monaural{
public:
		virtual AudioBuffer* makeMono(AudioBuffer*) throw (FatalException) = 0;
};

#endif
