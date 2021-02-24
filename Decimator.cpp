#include "Decimator.h"
#include <string.h>

int Decimator::setup(unsigned int factor, unsigned int blockSize)
{
	cleanup();
	this->blockSize = blockSize;
	this->factor = factor;
	this->numTaps = 9;
	
	source = (ne10_float32_t*) NE10_MALLOC (blockSize * sizeof (ne10_float32_t));
	destiny = (ne10_float32_t*) NE10_MALLOC ((blockSize*0.5) * sizeof (ne10_float32_t));
	if(!destiny || !source)
	{
		cleanup();
		return -1;
	}
	
	firFilterCoeff = (ne10_float32_t *) NE10_MALLOC (this->numTaps * sizeof (ne10_float32_t));
	firFilterState = (ne10_float32_t *) NE10_MALLOC ((this->numTaps + blockSize - 1) * sizeof (ne10_float32_t));
	if(!firFilterState || !firFilterCoeff)
		return -1;
		
	firFilterCoeff[0] = 0.009955733596956272; firFilterCoeff[1] = 0.04252038304633031; firFilterCoeff[2] = 0.09926778090651416; 
	firFilterCoeff[3] = 0.1575504280232556;   firFilterCoeff[4] = 0.18263400694813212; firFilterCoeff[5] = 0.1575504280232556; 
	firFilterCoeff[6] = 0.09926778090651416;  firFilterCoeff[7] = 0.04252038304633031; firFilterCoeff[8] = 0.009955733596956272; 

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
	ne10_fir_decimate_float_neon(&firFilter, source, destiny, blockSize); //const ne10_fir_decimate_instance_f32_t * , ne10_float32_t * pSrc, ne10_float32_t * pDst, ne10_uint32_t 	blockSize
	memcpy(output.data(), destiny, sizeof(destiny[0]) * blockSize);
	
	return output; 
}


void Decimator::cleanup()
{
	NE10_FREE(source);
	source = nullptr;
	NE10_FREE(destiny);
	destiny = nullptr;
}

