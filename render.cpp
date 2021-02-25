#include <Bela.h>
#include <libraries/AudioFile/AudioFile.h>
#include "Decimator.h"
#include <vector>
#include <string>
#include <algorithm>

Decimator decimator;
std::vector<float> gInBuf;
std::vector<float> gOutBuf;
const int gFactor = 2;

unsigned int gOutSampRate = 22050;
const double gDurationSec = 20; // how many seconds to record.
std::vector<std::vector<float>> gFileBuf(1, std::vector<float>(gOutSampRate * gDurationSec, 0));

std::string gOutFile = "decimator.wav";
unsigned int gOutFrames;
unsigned int gWrittenFrames = 0; // how many frames written to gOutBuf


bool setup(BelaContext *context, void *userData)
{
	unsigned int numFrames = gOutSampRate * gDurationSec;
	
	gFileBuf.resize(1);
	try {
		for(auto& c : gFileBuf)
			c.resize(numFrames);
	} catch (std::exception e) {
		fprintf(stderr, "Error while allocating memory. Maybe you are asking to record too many frames and/or too many channels\n");
		return false;
	}
	
	decimator.setup(gFactor, context->audioFrames);
	
	gInBuf.resize(context->audioFrames);
	gOutBuf.resize(context->audioFrames / gFactor);
	
	gOutFrames = context->audioFrames / gFactor;
	gOutSampRate = context->audioSampleRate / gFactor;
	
	return true;
}

void render(BelaContext *context, void *userData)
{
	for(unsigned int n = 0; n < context->audioFrames; n++) 
		gInBuf[n] = audioRead(context, n, 0);

	gOutBuf = decimator.decimate(gInBuf);
	
	for(unsigned int n = 0;  n < gOutFrames;  n++ ) {
		gFileBuf[0][gWrittenFrames] = gOutBuf[n];
		++gWrittenFrames;
		
		if(gWrittenFrames >= gFileBuf[0].size()) {
			Bela_requestStop();
			return;
		}
	}
}

void cleanup(BelaContext *context, void *userData)
{
	for(auto& o : gFileBuf)
		o.resize(gWrittenFrames);
	AudioFileUtilities::write(gOutFile, gFileBuf, gOutSampRate);
}