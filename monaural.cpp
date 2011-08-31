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

AudioStream* Monaural::makeMono(AudioStream* instrm){
	int c = instrm->getChannels();
	if(c == 1)
		return instrm;
	AudioStream* outstrm = new AudioStream();
	outstrm->setFrameRate(instrm->getFrameRate());
	outstrm->setChannels(1);
	outstrm->addToSampleCount(instrm->getSampleCount() / c);
	for(int i = 0; i < instrm->getSampleCount(); i += c){
		float mono = 0.0;
		for(int j = 0; j < c; j++)
			mono += instrm->getSample(i + j);
		mono /= c;
		outstrm->setSample(i/c,mono);
	}
	delete instrm;
	return outstrm;
}
