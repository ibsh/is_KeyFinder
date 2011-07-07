#include "downsamplerib.h"

/*
Decent raised cosine filter, designed using A J Fisher's tool at http://www-users.cs.york.ac.uk/~fisher/mkfilter
I need a filter that lowpasses under 1865 Hz (1760 plus a semitone, ceilinged), with the rolloff complete
before the new Nyquist (2205 Hz for a 10x downsample).
Those frequencies are 0.0423 and 0.05 of 44100, for the visualiser.

IMPORTANT:
The filter coefficients are all magic numbers, so this downsampler is only for 44100 fs, 10x ds.
*/

class Binode{
public:	
	Binode(float x = 0): l(NULL), r(NULL), n(x){}
	Binode* l, *r;
	float n;
};

AudioBuffer* IbDownsampler::downsample(AudioBuffer* inbuf, int factor) throw (FatalException){
	if(factor == 1) return inbuf;
	// prep output buffer
	AudioBuffer* outbuf = new AudioBuffer();
	outbuf->setFrameRate(inbuf->getFrameRate() / factor);
	outbuf->setChannels(inbuf->getChannels());
	int c = inbuf->getChannels();
	int ns = inbuf->getSampleCount() / factor;
	while(ns%c != 0) ns++;
	if(inbuf->getSampleCount() % factor > 0) ns += c;
	try{
		outbuf->addSamples(ns);
	}catch(FatalException){
		throw;
	}
	// prep filter
	int filterOrder = 40;
	float gain = 11.11103251;
	// create circular buffer for filter delay
	Binode* p = new Binode(); // first node
	Binode* q = p;
	for(int i=0; i<filterOrder; i++){
		q->r = new Binode(); // subsequent nodes
		q->r->l = q;
		q = q->r;
	}
	// join first and last nodes
	p->l = q;
	q->r = p;
	// filter coefficients
	float b[] = {
		-0.1558375039, -0.1874248693, -0.2086702773, -0.2169460753,
    -0.2101183762, -0.1866893664, -0.1459102136, -0.0878573589,
    -0.0134668982, +0.0754759816, +0.1763930677, +0.2860126960,
    +0.4005083622, +0.5156696485, +0.6270968347, +0.7304091364,
    +0.8214556276, +0.8965176433, +0.9524918178, +0.9870438931,
    +0.9987249707, +0.9870438931, +0.9524918178, +0.8965176433,
    +0.8214556276, +0.7304091364, +0.6270968347, +0.5156696485,
    +0.4005083622, +0.2860126960, +0.1763930677, +0.0754759816,
    -0.0134668982, -0.0878573589, -0.1459102136, -0.1866893664,
    -0.2101183762, -0.2169460753, -0.2086702773, -0.1874248693,
    -0.1558375039
	};
	for(int i=0; i<c; i++){ // for each channel (normally mono by this point but just in case)
		q = p;
		for(int k=0; k<=filterOrder; k++){ // clear delay buffer
			q->n = 0;
			q = q->r;
		}
		for(int j=i; j<inbuf->getSampleCount(); j+=c){ // for each frame
			p = p->r; // shuffle old samples along delay buffer
			p->l->n = inbuf->getSample(j) / gain; // load new sample into delay buffer
			if((j % (factor * c)) < c){ // only do the maths for the useful samples
				float sum = 0.0;
				q = p;
				for(int k=0; k<=filterOrder; k++){
					sum += b[k] * q->n;
					q = q->r;
				}
				outbuf->setSample((j / factor) + i, sum);
			}
		}
	}
	for(int k=0; k<=filterOrder; k++){
		q = p;
		p = p->r;
		delete q;
	}
	delete inbuf;
	return outbuf;
}
