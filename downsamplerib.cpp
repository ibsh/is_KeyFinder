#include "downsamplerib.h"

/*
Decent raised cosine filter, designed using A J Fisher's tool at http://www-users.cs.york.ac.uk/~fisher/mkfilter
I usually need a filter that lowpasses under 1760 Hz, with the rolloff complete before the new
Nyquist (2205 Hz for a 10x downsample). That's 0.05 of 44100, for the visualiser.

Input to Fisher's tool:
1st set of coefficients: 44100, 1760, 0, 41, raised-cosine response, no compensation function.
2nd set of coefficients: 44100, 2000, 0, 161, square root, no comp fn, hamming window.

IMPORTANT:
The filter coefficients are all magic numbers, so this downsampler is only for 44100 fs, 10x ds.
*/

class Binode{
public:	
	Binode(float x = 0): l(NULL), r(NULL), n(x){}
	Binode* l, *r;
	float n;
};

AudioBuffer* IbDownsampler::downsample(AudioBuffer* inbuf, int factor) throw (Exception){
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
	}catch(const Exception& e){
		throw e;
	}
	// prep filter
//	int filterOrder = 40;
//	float gain = 12.17345608;
		int filterOrder = 160;
		float gain = 11.03969310;
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
		// 1760, 0, 41, rcr.
//		-0.1911914604, -0.2097657151, -0.2166328392, -0.2100424818,
//		-0.1887493016, -0.1520965048, -0.1000738472, -0.0333465430,
//		+0.0467470824, +0.1382298360, +0.2385508491, +0.3446792163,
//		+0.4532221226, +0.5605622320, +0.6630080885, +0.7569505625,
//		+0.8390180039, +0.9062227535, +0.9560920270, +0.9867768959,
//		+0.9971341220, +0.9867768959, +0.9560920270, +0.9062227535,
//		+0.8390180039, +0.7569505625, +0.6630080885, +0.5605622320,
//		+0.4532221226, +0.3446792163, +0.2385508491, +0.1382298360,
//		+0.0467470824, -0.0333465430, -0.1000738472, -0.1520965048,
//		-0.1887493016, -0.2100424818, -0.2166328392, -0.2097657151,
//		-0.1911914604
		// 2000, 0, 161, sqrt, hamming.
		-0.0022979864, -0.0014851155, -0.0005276345, +0.0005287637,
		+0.0016288105, +0.0027066298, +0.0036859262, +0.0044820600,
		+0.0050064517, +0.0051734225, +0.0049091760, +0.0041622026,
		+0.0029140060, +0.0011887658, -0.0009395862, -0.0033443515,
		-0.0058483343, -0.0082321768, -0.0102489292, -0.0116443067,
		-0.0121813339, -0.0116673677, -0.0099809222, -0.0070953669,
		-0.0030964983, +0.0018087642, +0.0072947272, +0.0129315999,
		+0.0182126619, +0.0225928091, +0.0255360681, +0.0265684688,
		+0.0253317039, +0.0216323992, +0.0154816648, +0.0071199603,
		-0.0029768131, -0.0141127078, -0.0254095608, -0.0358661777,
		-0.0444356705, -0.0501157252, -0.0520448654, -0.0495965416,
		-0.0424622921, -0.0307153754, -0.0148472270, +0.0042291942,
		+0.0252127139, +0.0464845605, +0.0662137647, +0.0824916099,
		+0.0934864451, +0.0976077458, +0.0936666466, +0.0810194757,
		+0.0596811993, +0.0303971839, -0.0053357703, -0.0453047237,
		-0.0866737087, -0.1261316811, -0.1600878564, -0.1849028543,
		-0.1971406561, -0.1938239736, -0.1726744703, -0.1323195052,
		-0.0724487288, +0.0060931437, +0.1012868940, +0.2099971950,
		+0.3281078087, +0.4507269541, +0.5724509503, +0.6876697384,
		+0.7908945043, +0.8770856432, +0.9419588972, +0.9822487143,
		+0.9959106445, +0.9822487143, +0.9419588972, +0.8770856432,
		+0.7908945043, +0.6876697384, +0.5724509503, +0.4507269541,
		+0.3281078087, +0.2099971950, +0.1012868940, +0.0060931437,
		-0.0724487288, -0.1323195052, -0.1726744703, -0.1938239736,
		-0.1971406561, -0.1849028543, -0.1600878564, -0.1261316811,
		-0.0866737087, -0.0453047237, -0.0053357703, +0.0303971839,
		+0.0596811993, +0.0810194757, +0.0936666466, +0.0976077458,
		+0.0934864451, +0.0824916099, +0.0662137647, +0.0464845605,
		+0.0252127139, +0.0042291942, -0.0148472270, -0.0307153754,
		-0.0424622921, -0.0495965416, -0.0520448654, -0.0501157252,
		-0.0444356705, -0.0358661777, -0.0254095608, -0.0141127078,
		-0.0029768131, +0.0071199603, +0.0154816648, +0.0216323992,
		+0.0253317039, +0.0265684688, +0.0255360681, +0.0225928091,
		+0.0182126619, +0.0129315999, +0.0072947272, +0.0018087642,
		-0.0030964983, -0.0070953669, -0.0099809222, -0.0116673677,
		-0.0121813339, -0.0116443067, -0.0102489292, -0.0082321768,
		-0.0058483343, -0.0033443515, -0.0009395862, +0.0011887658,
		+0.0029140060, +0.0041622026, +0.0049091760, +0.0051734225,
		+0.0050064517, +0.0044820600, +0.0036859262, +0.0027066298,
		+0.0016288105, +0.0005287637, -0.0005276345, -0.0014851155,
		-0.0022979864
	};
	for(int i=0; i<c; i++){ // for each channel (normally mono by this point but just in case)
		q = p;
		for(int k=0; k<=filterOrder; k++){ // clear delay buffer
			q->n = 0.0;
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
