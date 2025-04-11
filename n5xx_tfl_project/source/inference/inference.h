#ifndef INFERENCE_H
#define INFERENCE_H

#include <cstdint>

#define USE_NO_QUANTIIZATION 1
//#define USE_QUANTIZATION 1
//#define USE_QUANTIZATION_NEUTRON 1

#ifdef USE_NO_QUANTIIZATION
typedef float inference_data_type;
#endif //USE_NO_QUANTIIZATION

#ifdef USE_QUANTIZATION
typedef uint8_t inference_data_type;
#endif //USE_NO_QUANTIIZATION

#ifdef USE_QUANTIZATION_NEUTRON
typedef uint8_t inference_data_type;
#endif // USE_QUANTIZATION_NEUTRON

void
inference_setup(void);

void
inference_set_input(
	const inference_data_type* input_buffer,
	const uint32_t input_buffer_length);


void
inference_get_output(
	inference_data_type* output_buffer,
	const uint32_t output_buffer_length);

void
inference_predict();

#endif //INFERENCE_H
