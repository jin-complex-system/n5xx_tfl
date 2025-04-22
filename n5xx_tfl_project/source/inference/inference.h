#ifndef INFERENCE_H
#define INFERENCE_H

#include <cstdint>

//#define CNN_MODEL_NO_NEUTRON 1
#define CNN_MODEL_NEUTRON 1
//#define DTFT_MODEL_NO_NEUTRON 1
//#define DTFT_MODEL_NEUTRON 1

typedef uint8_t inference_data_type;

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
