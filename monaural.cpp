/*************************************************************************

	Copyright 2011 Ibrahim Sha'ath

	This file is part of KeyFinder.

	KeyFinder is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	KeyFinder is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with KeyFinder.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#include "monaural.h"

AudioBuffer* Monaural::makeMono(AudioBuffer* inbuf){
	int c = inbuf->getChannels();
	if(c == 1)
		return inbuf;
	AudioBuffer* outbuf = new AudioBuffer();
	outbuf->setFrameRate(inbuf->getFrameRate());
	outbuf->setChannels(1);
	outbuf->addSamples(inbuf->getSampleCount() / c);
	for(int i = 0; i < inbuf->getSampleCount(); i += c){
		float mono = 0.0;
		for(int j = 0; j < c; j++)
			mono += inbuf->getSample(i + j);
		mono /= c;
		outbuf->setSample(i/c,mono);
	}
	delete inbuf;
	return outbuf;
}
