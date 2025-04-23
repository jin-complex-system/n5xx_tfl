#ifndef INFERENCE_DEFINITIONS_H_
#define INFERENCE_DEFINITIONS_H_

#include <cstdint>

typedef uint8_t inference_input_data_type;
typedef uint8_t inference_output_data_type;

#define USE_TENSORFLOW 1

constexpr
uint32_t
NUM_CLASSES = 10;

constexpr
uint32_t
INFERENCE_INPUT_ARRAY_LENGTH = (32 * 32);

#endif /* INFERENCE_DEFINITIONS_H_ */
