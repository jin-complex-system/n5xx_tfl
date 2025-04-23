#ifndef INFERENCE_H
#define INFERENCE_H

#include "inference_definitions.h"

#if USE_TENSORFLOW
#include "inference_tf.h"
#endif // USE_TENSORFLOW

/**
 * Setup inference
 */
inline
void
inference_setup(void) {
#if USE_TENSORFLOW
	inference_tf_setup();
#endif // USE_TENSORFLOW
}

/**
 * Set input buffer
 *
 * @param input_buffer
 * @param input_buffer_length
 */
inline
void
inference_set_input(
	const inference_input_data_type* input_buffer,
	const uint32_t input_buffer_length) {
#if USE_TENSORFLOW
	inference_tf_set_input(
			input_buffer,
			input_buffer_length);
#endif // USE_TENSORFLOW
}

/**
 * Get the output buffer
 *
 * @param output_buffer
 * @param output_buffer_length
 */
inline
void
inference_get_output(
	inference_output_data_type* output_buffer,
	const uint32_t output_buffer_length) {
#if USE_TENSORFLOW
	inference_tf_get_output(
			output_buffer,
			output_buffer_length);
#endif // USE_TENSORFLOW
}

/**
 * Perform prediction
 */
inline
void
inference_predict(void) {
#if USE_TENSORFLOW
	inference_tf_predict();
#endif // USE_TENSORFLOW
}

#endif // INFERENCE_H
