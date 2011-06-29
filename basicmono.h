#ifndef BASICMONO_H
#define BASICMONO_H

#include "monaural.h"

class basicMono : public Monaural{
public:
		virtual AudioBuffer* makeMono(AudioBuffer*) throw (FatalException);
};

#endif
