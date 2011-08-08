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

#include "downsamplerlibsrc.h"
#include <samplerate.h>

AudioBuffer* SecretRabbitDownsampler::downsample(AudioBuffer* inbuf, int factor) throw (Exception){
	if(factor == 1) return inbuf;
	AudioBuffer* outbuf = new AudioBuffer();
	outbuf->setFrameRate(inbuf->getFrameRate() / factor);
	outbuf->setChannels(inbuf->getChannels());
	try{
		outbuf->addSamples(inbuf->getSampleCount() / factor);
	}catch(const Exception& e){
		throw e;
	}
	SRC_DATA srcData;
	srcData.data_in = &inbuf->buffer.front();
	srcData.data_out = &outbuf->buffer.front();
	srcData.input_frames = (long)(inbuf->getSampleCount() / inbuf->getChannels());
	srcData.output_frames = (long)(outbuf->getSampleCount() / outbuf->getChannels());
	srcData.src_ratio = 1.0/factor;
	// second parameter = converter type.
	// 0-2 quite slow but they anti-alias roughly the same amount, based on a visual inspection.
	// 3-4 super fast but the aliasing is worse than a simple discard. Don't use.
	int result = src_simple(&srcData,2,inbuf->getChannels());
	if(result != 0){
		qCritical("Error in LibSRC sample rate conversion");
		throw Exception();
	}
	delete inbuf;
	return outbuf;
}
