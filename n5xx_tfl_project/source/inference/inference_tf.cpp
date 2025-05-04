#include <inference/inference_tf.h>
#include <cassert>
#include <tensorflow/lite/micro/kernels/micro_ops.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/micro_op_resolver.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <tensorflow/lite/micro/kernels/neutron/neutron.h>
#include "fsl_debug_console.h"

#ifdef CNN_MODEL_NO_NEUTRON
#include "models/CNN_litert.h"

static
tflite::MicroMutableOpResolver<6>
s_microOpResolver;

static inline
void
model_GetOpsResolver(void) {
	s_microOpResolver.AddQuantize();
	s_microOpResolver.AddConv2D();
	s_microOpResolver.AddMaxPool2D();
	s_microOpResolver.AddReshape();
	s_microOpResolver.AddFullyConnected();
	s_microOpResolver.AddSoftmax();
}
#endif // CNN_MODEL_NO_NEUTRON

#ifdef CNN_MODEL_NEUTRON
#include "models/CNN_litert_neutron.h"

static
tflite::MicroMutableOpResolver<3>
s_microOpResolver;

static inline
void
model_GetOpsResolver(void) {
	s_microOpResolver.AddQuantize();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddCustom(tflite::GetString_NEUTRON_GRAPH(), tflite::Register_NEUTRON_GRAPH());
}

#endif // CNN_MODEL_NEUTRON

#ifdef DTFT_MODEL_NO_NEUTRON
#include "models/DTFT.h"

static
tflite::MicroMutableOpResolver<14>
s_microOpResolver;

static inline
void
model_GetOpsResolver(void) {
	s_microOpResolver.AddQuantize();
	s_microOpResolver.AddTranspose();
	s_microOpResolver.AddFullyConnected();
	s_microOpResolver.AddAdd();
	s_microOpResolver.AddMean();
	s_microOpResolver.AddSquaredDifference();
	s_microOpResolver.AddRsqrt();
	s_microOpResolver.AddMul();
	s_microOpResolver.AddSub();
	s_microOpResolver.AddReshape();
	s_microOpResolver.AddBatchMatMul();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddConv2D();
	s_microOpResolver.AddConcatenation();
}

#endif // DTFT_MODEL_NO_NEUTRON


#ifdef DTFT_MODEL_NEUTRON
#include "models/DTFT_neutron.h"

static
tflite::MicroMutableOpResolver<9>
s_microOpResolver;

static inline
void
model_GetOpsResolver(void) {
	s_microOpResolver.AddQuantize();
	s_microOpResolver.AddTranspose();
	s_microOpResolver.AddMean();
	s_microOpResolver.AddSquaredDifference();
	s_microOpResolver.AddAdd();
	s_microOpResolver.AddRsqrt();
	s_microOpResolver.AddMul();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddCustom(tflite::GetString_NEUTRON_GRAPH(), tflite::Register_NEUTRON_GRAPH());
}

#endif // DTFT_MODEL_NEUTRON

#ifdef DTFT_SAC_NO_NEUTRON
#include "models/DTFT_SAC.h"

static
tflite::MicroMutableOpResolver<13>
s_microOpResolver;

static inline
void
model_GetOpsResolver(void) {
	s_microOpResolver.AddReshape();
	s_microOpResolver.AddTranspose();
	s_microOpResolver.AddFullyConnected();
	s_microOpResolver.AddAdd();
	s_microOpResolver.AddMean();
	s_microOpResolver.AddSquaredDifference();
	s_microOpResolver.AddRsqrt();
	s_microOpResolver.AddMul();
	s_microOpResolver.AddSub();
	s_microOpResolver.AddBatchMatMul();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddConcatenation();
	s_microOpResolver.AddLogistic();
}

#endif // DTFT_SAC_NO_NEUTRON

#ifdef DTFT_SAC_NEUTRON
#include "models/DTFT_SAC_neutron.h"

static
tflite::MicroMutableOpResolver<10>
s_microOpResolver;

static inline
void
model_GetOpsResolver(void) {
	s_microOpResolver.AddReshape();
	s_microOpResolver.AddTranspose();
	s_microOpResolver.AddMean();
	s_microOpResolver.AddSquaredDifference();
	s_microOpResolver.AddAdd();
	s_microOpResolver.AddRsqrt();
	s_microOpResolver.AddMul();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddLogistic();
	s_microOpResolver.AddCustom(tflite::GetString_NEUTRON_GRAPH(), tflite::Register_NEUTRON_GRAPH());
}

#endif // DTFT_SAC_NEUTRON

#ifdef FT_MODEL_NO_NEUTRON
#include "models/FT.h"

static
tflite::MicroMutableOpResolver<20>
s_microOpResolver;

static inline
void
model_GetOpsResolver(void) {
	s_microOpResolver.AddQuantize();
	s_microOpResolver.AddShape();
	s_microOpResolver.AddStridedSlice();
	s_microOpResolver.AddPack();
	s_microOpResolver.AddReshape();
	s_microOpResolver.AddGather();
//	s_microOpResolver.AddReduceProd(); // Does not Exist
	s_microOpResolver.AddConcatenation();
	s_microOpResolver.AddFullyConnected();
	s_microOpResolver.AddAdd();
	s_microOpResolver.AddMean();
	s_microOpResolver.AddSquaredDifference();
	s_microOpResolver.AddRsqrt();
	s_microOpResolver.AddMul();
	s_microOpResolver.AddSub();
	s_microOpResolver.AddTranspose();
	s_microOpResolver.AddBatchMatMul();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddExpandDims();
	s_microOpResolver.AddConv2D();
}

#endif // FT_MODEL_NO_NEUTRON

#ifdef FT_MODEL_NEUTRON
#include "models/FT_neutron.h"

static
tflite::MicroMutableOpResolver<9>
s_microOpResolver;

static inline
void
model_GetOpsResolver(void) {
	s_microOpResolver.AddQuantize();
	s_microOpResolver.AddMean();
	s_microOpResolver.AddSquaredDifference();
	s_microOpResolver.AddAdd();
	s_microOpResolver.AddRsqrt();
	s_microOpResolver.AddMul();
	s_microOpResolver.AddTranspose();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddCustom(tflite::GetString_NEUTRON_GRAPH(), tflite::Register_NEUTRON_GRAPH());
}

#endif // FT_MODEL_NEUTRON

#ifdef TFT_MODEL_NO_NEUTRON
#include "models/TFT.h"

static
tflite::MicroMutableOpResolver<19>
s_microOpResolver;

static inline
void
model_GetOpsResolver(void) {
	s_microOpResolver.AddQuantize();
	s_microOpResolver.AddShape();
	s_microOpResolver.AddGather();
//	s_microOpResolver.AddReduceProd(); // Does not exist
	s_microOpResolver.AddPack();
	s_microOpResolver.AddReshape();
	s_microOpResolver.AddConcatenation();
	s_microOpResolver.AddFullyConnected();
	s_microOpResolver.AddAdd();
	s_microOpResolver.AddMean();
	s_microOpResolver.AddSquaredDifference();
	s_microOpResolver.AddRsqrt();
	s_microOpResolver.AddMul();
	s_microOpResolver.AddSub();
	s_microOpResolver.AddTranspose();
	s_microOpResolver.AddBatchMatMul();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddExpandDims();
	s_microOpResolver.AddConv2D();
}

#endif // TFT_MODEL_NO_NEUTRON

#ifdef TFT_MODEL_NEUTRON
#include "models/TFT_neutron.h"

static
tflite::MicroMutableOpResolver<9>
s_microOpResolver;

static inline
void
model_GetOpsResolver(void) {
	s_microOpResolver.AddQuantize();
	s_microOpResolver.AddMean();
	s_microOpResolver.AddSquaredDifference();
	s_microOpResolver.AddAdd();
	s_microOpResolver.AddRsqrt();
	s_microOpResolver.AddMul();
	s_microOpResolver.AddTranspose();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddCustom(tflite::GetString_NEUTRON_GRAPH(), tflite::Register_NEUTRON_GRAPH());
}

#endif // FT_MODEL_NEUTRON

#ifdef DTFT_MODEL_FLOAT_NO_NEUTRON
#include "models/DTFT_reduced_params_float.h"

static
tflite::MicroMutableOpResolver<13>
s_microOpResolver;

static inline
void
model_GetOpsResolver(void) {
	s_microOpResolver.AddFullyConnected();
	s_microOpResolver.AddTranspose();
	s_microOpResolver.AddAdd();
	s_microOpResolver.AddMean();
	s_microOpResolver.AddSquaredDifference();
	s_microOpResolver.AddRsqrt();
	s_microOpResolver.AddMul();
	s_microOpResolver.AddSub();
	s_microOpResolver.AddReshape();
	s_microOpResolver.AddBatchMatMul();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddConv2D();
	s_microOpResolver.AddConcatenation();
}

#endif // DTFT_MODEL_FLOAT_NO_NEUTRON

#ifdef DTFT_MODEL_FLOAT_NEUTRON
#include "models/DTFT_reduced_params_neutron_float.h"

static
tflite::MicroMutableOpResolver<13>
s_microOpResolver;

static inline
void
model_GetOpsResolver(void) {
	s_microOpResolver.AddFullyConnected();
	s_microOpResolver.AddTranspose();
	s_microOpResolver.AddAdd();
	s_microOpResolver.AddMean();
	s_microOpResolver.AddSquaredDifference();
	s_microOpResolver.AddRsqrt();
	s_microOpResolver.AddMul();
	s_microOpResolver.AddSub();
	s_microOpResolver.AddReshape();
	s_microOpResolver.AddBatchMatMul();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddConv2D();
	s_microOpResolver.AddConcatenation();
}

#endif // DTFT_MODEL_FLOAT_NEUTRON

static const
tflite::Model*
s_model = nullptr;

static
tflite::MicroInterpreter*
s_interpreter = nullptr;

/// An area of memory to use for input, output, and intermediate arrays.
/// (Can be adjusted based on the model needs.)
constexpr uint32_t
kTensorArenaSize = 350 * 1024;

#ifdef TENSORARENA_NONCACHE
static uint8_t classifier_tensorArena[kTensorArenaSize] __ALIGNED(16) __attribute__((section("NonCacheable")));
#else
static
uint8_t
s_tensorArena[kTensorArenaSize] __attribute__((aligned(16)));
#endif // TENSORARENA_NONCACHE

void
inference_tf_setup(void) {
	s_model = tflite::GetModel(model_data);
	assert(s_model->version() == TFLITE_SCHEMA_VERSION);

    /// Pull in only the operation implementations we need.
    /// This relies on a complete list of all the ops needed by this graph.
    /// NOLINTNEXTLINE(runtime-global-variables)
    model_GetOpsResolver();

    /// Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(
		s_model,
		s_microOpResolver,
		s_tensorArena,
        kTensorArenaSize);
    s_interpreter = &static_interpreter;

    /// Allocate memory from the tensor_arena for the model's tensors.
    const TfLiteStatus allocate_status =
    		s_interpreter->AllocateTensors();
    assert(allocate_status == kTfLiteOk);

    const auto num_bytes_used = s_interpreter->arena_used_bytes();
    PRINTF("Number of bytes actually used for %s: %lu\r\n", MODEL_NAME, (uint32_t)num_bytes_used);
}

void
inference_tf_set_input(
	const inference_input_data_type* input_buffer,
	const uint32_t input_buffer_length) {
	/// Check parameters
	{
		assert(input_buffer != nullptr);
		assert(input_buffer_length > 0);
	}

	assert(s_interpreter != nullptr);
	TfLiteTensor* inputTensor = s_interpreter->input(0);
	assert(inputTensor != nullptr);
	assert(inputTensor->type == EXPECTED_INPUT_DATA_TYPE);

#ifndef NDEBUG
	uint32_t tensor_size = 1;
	for (int dims_iterator = 0; dims_iterator < inputTensor->dims->size; dims_iterator++) {
		tensor_size = tensor_size * inputTensor->dims->data[dims_iterator];
	}
	assert(input_buffer_length == tensor_size);
#endif // NDEBUG

	inference_input_data_type* tensor_input;
	if (EXPECTED_INPUT_DATA_TYPE == kTfLiteUInt8) {
		tensor_input = (inference_input_data_type *)inputTensor->data.uint8;
	}
	else if (EXPECTED_INPUT_DATA_TYPE == kTfLiteFloat32) {
		tensor_input = (inference_input_data_type *)inputTensor->data.f;
	}
	else if (EXPECTED_INPUT_DATA_TYPE == kTfLiteInt8) {
		tensor_input = (inference_input_data_type *)inputTensor->data.int8;
	}
	assert(tensor_input != nullptr);

	/// Load input
    {
        for (uint32_t tensor_iterator = 0; tensor_iterator < input_buffer_length; tensor_iterator++) {
        	tensor_input[tensor_iterator] =
        			input_buffer[tensor_iterator];
        }
    }
}

void
inference_tf_get_output(
	inference_output_data_type* output_buffer,
	const uint32_t output_buffer_length) {
	/// Check parameters
	{
		assert(output_buffer != nullptr);
		assert(output_buffer_length > 0);
	}

	assert(s_interpreter != nullptr);

	TfLiteTensor* outputtTensor;
	outputtTensor = s_interpreter->output(0);

#ifdef DTFT_SAC_NO_NEUTRON
	outputtTensor = s_interpreter->output(1);
#endif

#ifdef DTFT_SAC_NEUTRON
	outputtTensor = s_interpreter->output(1);
#endif


    assert(outputtTensor != nullptr);
    assert(outputtTensor->type == EXPECTED_OUTPUT_DATA_TYPE);

#ifndef NDEBUG
	uint32_t tensor_size = 1;
	for (int dims_iterator = 0; dims_iterator < outputtTensor->dims->size; dims_iterator++) {
		tensor_size = tensor_size * outputtTensor->dims->data[dims_iterator];
	}
	assert(output_buffer_length == tensor_size);
#endif // NDEBUG

	inference_output_data_type* tensor_output;
	if (EXPECTED_OUTPUT_DATA_TYPE == kTfLiteUInt8) {
		tensor_output = (inference_output_data_type *)outputtTensor->data.uint8;
	}
	else if (EXPECTED_OUTPUT_DATA_TYPE == kTfLiteFloat32) {
		tensor_output = (inference_output_data_type *)outputtTensor->data.f;
	}
	else if (EXPECTED_OUTPUT_DATA_TYPE == kTfLiteInt8) {
		tensor_output = (inference_input_data_type *)outputtTensor->data.int8;
	}
	assert(tensor_output != nullptr);

    /// Unload output buffer
    {
        for (uint32_t tensor_iterator = 0; tensor_iterator < output_buffer_length; tensor_iterator++) {
        	inference_output_data_type my_output = tensor_output[tensor_iterator];
        	output_buffer[tensor_iterator] = my_output;
        }
    }
}

void
inference_tf_predict() {
	const auto tflite_status =
			s_interpreter->Invoke();
	assert(tflite_status == kTfLiteOk);
}


