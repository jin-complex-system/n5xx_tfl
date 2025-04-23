#include <inference/inference_tf.h>

#ifdef CNN_MODEL_NO_NEUTRON
#include "models/CNN_model.h"
#endif // CNN_MODEL_NO_NEUTRON

#ifdef CNN_MODEL_NEUTRON
#include "models/CNN_model_neutron.h"
#endif // CNN_MODEL_NEUTRON

#ifdef DTFT_MODEL_NO_NEUTRON
#include "models/DTFT_model.h"
#endif // DTFT_MODEL_NO_NEUTRON

#ifdef DTFT_MODEL_NEUTRON
#include "models/DTFT_model_neutron.h"
#endif // DTFT_MODEL_NEUTRON

#include <cassert>
#include <tensorflow/lite/micro/kernels/micro_ops.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/micro_op_resolver.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <tensorflow/lite/micro/kernels/neutron/neutron.h>

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

#ifdef CNN_MODEL_NO_NEUTRON
static
tflite::MicroMutableOpResolver<6>
s_microOpResolver;
#endif // CNN_MODEL_NO_NEUTRON

#ifdef CNN_MODEL_NEUTRON
static
tflite::MicroMutableOpResolver<3>
s_microOpResolver;
#endif // CNN_MODEL_NEUTRON

#ifdef DTFT_MODEL_NO_NEUTRON
static
tflite::MicroMutableOpResolver<20>
s_microOpResolver;
#endif // DTFT_MODEL_NO_NEUTRON

#ifdef DTFT_MODEL_NEUTRON
static
tflite::MicroMutableOpResolver<10>
s_microOpResolver;
#endif // DTFT_MODEL_NEUTRON

static inline
void
model_GetOpsResolver(void) {
#ifdef CNN_MODEL_NO_NEUTRON
	s_microOpResolver.AddQuantize();
	s_microOpResolver.AddConv2D();
	s_microOpResolver.AddMaxPool2D();
	s_microOpResolver.AddReshape();
	s_microOpResolver.AddFullyConnected();
	s_microOpResolver.AddSoftmax();
#endif // CNN_MODEL_NO_NEUTRON

#ifdef CNN_MODEL_NEUTRON
	s_microOpResolver.AddQuantize();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddCustom(tflite::GetString_NEUTRON_GRAPH(), tflite::Register_NEUTRON_GRAPH());
#endif // CNN_MODEL_NEUTRON

#ifdef DTFT_MODEL_NO_NEUTRON
	s_microOpResolver.AddQuantize();
	s_microOpResolver.AddShape();
	s_microOpResolver.AddStridedSlice();
	s_microOpResolver.AddPack();
	s_microOpResolver.AddReshape();
	s_microOpResolver.AddGather();
//	s_microOpResolver.AddReduceProd(); // Does not exist
	s_microOpResolver.AddReduceMax();
	s_microOpResolver.AddTranspose();
	s_microOpResolver.AddConcatenation();
	s_microOpResolver.AddFullyConnected();
	s_microOpResolver.AddAdd();
	s_microOpResolver.AddMean();
	s_microOpResolver.AddSquaredDifference();
	s_microOpResolver.AddRsqrt();
	s_microOpResolver.AddMul();
	s_microOpResolver.AddSub();
	s_microOpResolver.AddBatchMatMul();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddExpandDims();
	s_microOpResolver.AddConv2D();
#endif // DTFT_MODEL_NO_NEUTRON

#ifdef DTFT_MODEL_NEUTRON
	s_microOpResolver.AddQuantize();
	s_microOpResolver.AddReshape();
	s_microOpResolver.AddTranspose();
	s_microOpResolver.AddMean();
	s_microOpResolver.AddSquaredDifference();
	s_microOpResolver.AddAdd();
	s_microOpResolver.AddRsqrt();
	s_microOpResolver.AddMul();
	s_microOpResolver.AddSoftmax();
	s_microOpResolver.AddCustom(tflite::GetString_NEUTRON_GRAPH(), tflite::Register_NEUTRON_GRAPH());
#endif // DTFT_MODEL_NEUTRON
}

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
    TfLiteTensor* outputtTensor = s_interpreter->output(0);
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


