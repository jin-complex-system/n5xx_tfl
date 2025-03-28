#ifndef INFERENCE_H
#define INFERENCE_H

#include <cstdint>

void
inference_setup(void);

void
inference_set_input(
	const float* input_buffer,
	const uint32_t input_buffer_length);


void
inference_get_output(
	float* output_buffer,
	const uint32_t output_buffer_length);

void
inference_predict();

#endif //INFERENCE_H
