#include <Bela.h>

#include "Decimator.h"
#include <vector>

Decimator decimator;
std::vector<float> inBuf;
std::vector<float> outBuf;

bool setup(BelaContext *context, void *userData)
{
	decimator.setup(2, context->audioFrames);
	inBuf.resize(context->audioFrames);
	outBuf.resize(context->audioFrames * 0.5);
	
	return true;
}

void render(BelaContext *context, void *userData)
{
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		inBuf[n] = audioRead(context, n, 0);
	}
	outBuf = decimator.decimate(inBuf);
	
	
	// dirty test
	unsigned int flip = 0; 
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		if(flip == 0) {
			flip = 1;
			audioWrite(context, n, 0, outBuf[n]);	
		}
		else {
			flip = 0;
			audioWrite(context, n, 0, outBuf[n-1]);
		}
		
	}

}

void cleanup(BelaContext *context, void *userData)
{

}