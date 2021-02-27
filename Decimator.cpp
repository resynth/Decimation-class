#include "Decimator.h"
#include <string.h>

int Decimator::setup(unsigned int factor, unsigned int blockSize)
{
	cleanup();
	this->blockSize = blockSize;
	this->factor = factor;

	if(factor == 2) this->numTaps = 117;
	else if(factor == 4) this->numTaps = 229;
	//else rt_printf("gFactor can be 2 or 4.  Currently set to %i", factor)
	
	firFilterCoeff = (ne10_float32_t *) NE10_MALLOC (this->numTaps * sizeof (ne10_float32_t));
	firFilterState = (ne10_float32_t *) NE10_MALLOC ((this->numTaps + blockSize - 1) * sizeof (ne10_float32_t));
	if(!firFilterState || !firFilterCoeff)
		return -1;
		
	if(factor == 2) firFilterCoeff = factor2Coeff;
	else if(factor == 4) firFilterCoeff = factor4Coeff;
	
	outSize = blockSize / factor;
	output.resize(outSize);
	source = (ne10_float32_t*) NE10_MALLOC (blockSize * sizeof (ne10_float32_t));
	destiny = (ne10_float32_t*) NE10_MALLOC ((outSize) * sizeof (ne10_float32_t));
	if(!destiny || !source)
	{
		cleanup();
		return -1;
	}

	ne10_fir_decimate_init_float (	
		&firFilter,
		this->numTaps,
		this->factor,
		firFilterCoeff,
		firFilterState,
		blockSize 
	);	
	
	return 0;
}

std::vector<float>& Decimator::decimate(std::vector<float>& input) 
{
	memcpy(source, input.data(), sizeof(input[0]) * blockSize);
	ne10_fir_decimate_float_neon(&firFilter, source, destiny, blockSize);
	memcpy(output.data(), destiny, sizeof(destiny[0]) * (outSize));
	return output; 
}

void Decimator::cleanup()
{
	NE10_FREE(source);
	source = nullptr;
	NE10_FREE(destiny);
	destiny = nullptr;
}

