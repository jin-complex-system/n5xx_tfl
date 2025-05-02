#ifndef INFERENCE_DEFINITIONS_H_
#define INFERENCE_DEFINITIONS_H_

#include <cstdint>

#define USE_TENSORFLOW 1
#ifdef USE_TENSORFLOW
#include "tensorflow/lite/c/common.h"
#endif // USE_TENSORFLOW

#define USE_QUANTIZED_MODELS 1
#ifdef USE_QUANTIZED_MODELS

typedef int8_t inference_input_data_type;
typedef int8_t inference_output_data_type;

constexpr TfLiteType
EXPECTED_INPUT_DATA_TYPE = kTfLiteInt8;
constexpr TfLiteType
EXPECTED_OUTPUT_DATA_TYPE = kTfLiteInt8;

#else

typedef float inference_input_data_type;
typedef float inference_output_data_type;

constexpr TfLiteType
EXPECTED_INPUT_DATA_TYPE = kTfLiteFloat32;
constexpr TfLiteType
EXPECTED_OUTPUT_DATA_TYPE = kTfLiteFloat32;

#endif // USE_QUANTIZED_MODELS

constexpr
uint32_t
NUM_CLASSES = 10;

constexpr
uint32_t
INFERENCE_INPUT_ARRAY_LENGTH = (32 * 32);

#endif /* INFERENCE_DEFINITIONS_H_ */
