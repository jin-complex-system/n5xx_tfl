#ifndef INFERENCE_TF_H
#define INFERENCE_TF_H

#include <cstdint>
#include "inference_definitions.h"

#ifdef USE_QUANTIZED_MODELS
//#define CNN_MODEL_NO_NEUTRON 1
//#define CNN_MODEL_NEUTRON 1
//#define DTFT_MODEL_NO_NEUTRON 1
//#define DTFT_MODEL_NEUTRON 1
//#define DTFT_SAC_NO_NEUTRON 1
//#define DTFT_SAC_NEUTRON 1
//#define FT_MODEL_NO_NEUTRON 1
#define FT_MODEL_NEUTRON 1
//#define TFT_MODEL_NO_NEUTRON 1
//#define TFT_MODEL_NEUTRON 1
#else
//#define DTFT_MODEL_FLOAT_NO_NEUTRON 1
//#define DTFT_MODEL_FLOAT_NEUTRON 1
#endif // USE_QUANTIZED_MODELS

/**
 * Setup inference using TensorFlow
 */
void
inference_tf_setup(void);

/**
 * Set input buffer using TensorFlow
 *
 * @param input_buffer
 * @param input_buffer_length
 */
void
inference_tf_set_input(
	const inference_input_data_type* input_buffer,
	const uint32_t input_buffer_length);

/**
 * Get the output buffer using TensorFlow
 *
 * @param output_buffer
 * @param output_buffer_length
 */
void
inference_tf_get_output(
	inference_output_data_type* output_buffer,
	const uint32_t output_buffer_length);

/**
 * Perform prediction using TensorFlow
 */
void
inference_tf_predict();

#endif //INFERENCE_TF_H
