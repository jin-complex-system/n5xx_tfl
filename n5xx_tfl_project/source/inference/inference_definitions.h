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

#ifdef USE_TENSORFLOW
#include "tensorflow/lite/c/common.h"

constexpr TfLiteType
EXPECTED_INPUT_DATA_TYPE = kTfLiteUInt8;

constexpr TfLiteType
EXPECTED_OUTPUT_DATA_TYPE = kTfLiteUInt8;

#endif // USE_TENSORFLOW

#endif /* INFERENCE_DEFINITIONS_H_ */
