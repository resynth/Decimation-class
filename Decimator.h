#define ENABLE_NE10_FIR_DECIMATE_FLOAT_NEON
#include <libraries/ne10/NE10.h>
#include <vector>


class Decimator
{
public:
		Decimator(){};
		Decimator(unsigned int factor, unsigned int blocksize){ setup(factor, blocksize); };
		int setup(unsigned int factor, unsigned int blocksize);
		
		std::vector<float> output;
		std::vector<float>& decimate(std::vector<float>& input);
		
		~Decimator(){ cleanup(); };
		void cleanup();
	
private:
		ne10_fir_decimate_instance_f32_t	firFilter;	
		ne10_float32_t *  					source = nullptr;		
		ne10_float32_t *  					destiny = nullptr;		
		ne10_uint32_t  						blockSize;				
		ne10_uint8_t						factor;						
		ne10_uint16_t						numTaps;			
		ne10_float32_t*						firFilterCoeff;			
		ne10_float32_t*						firFilterState;	// Points to state variable array of length numTaps+maxBlockSize-1
};