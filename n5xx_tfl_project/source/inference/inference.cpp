#include "inference.h"
#include "models/DTFT_model.h"

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
kTensorArenaSize = 300 * 1024;

#ifdef TENSORARENA_NONCACHE
static uint8_t classifier_tensorArena[kTensorArenaSize] __ALIGNED(16) __attribute__((section("NonCacheable")));
#else
static
uint8_t
s_tensorArena[kTensorArenaSize] __attribute__((aligned(16)));
#endif // TENSORARENA_NONCACHE

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

void
inference_setup(void) {
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
inference_set_input(
	const float* input_buffer,
	const uint32_t input_buffer_length) {
	/// Check parameters
	{
		assert(input_buffer != nullptr);
		assert(input_buffer_length > 0);
	}

	assert(s_interpreter != nullptr);
	TfLiteTensor* inputTensor = s_interpreter->input(0);
	assert(inputTensor != nullptr);
	assert(inputTensor->type == kTfLiteFloat32);

#ifndef NDEBUG
	uint32_t tensor_size = 1;
	for (int dims_iterator = 0; dims_iterator < inputTensor->dims->size; dims_iterator++) {
		tensor_size = tensor_size * inputTensor->dims->data[dims_iterator];
	}
	assert(input_buffer_length == tensor_size);
#endif // NDEBUG

	float* tensor_input = inputTensor->data.f;
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
inference_get_output(
	float* output_buffer,
	const uint32_t output_buffer_length) {
	/// Check parameters
	{
		assert(output_buffer != nullptr);
		assert(output_buffer_length > 0);
	}

	assert(s_interpreter != nullptr);
    TfLiteTensor* outputtTensor = s_interpreter->output(0);
    assert(outputtTensor != nullptr);
    assert(outputtTensor->type == kTfLiteFloat32);

#ifndef NDEBUG
	uint32_t tensor_size = 1;
	for (int dims_iterator = 0; dims_iterator < outputtTensor->dims->size; dims_iterator++) {
		tensor_size = tensor_size * outputtTensor->dims->data[dims_iterator];
	}
	assert(output_buffer_length == tensor_size);
#endif // NDEBUG

	float* tensor_output = outputtTensor->data.f;
	assert(tensor_output != nullptr);

    /// Unload output buffer
    {
        for (uint32_t tensor_iterator = 0; tensor_iterator < output_buffer_length; tensor_iterator++) {
        	float my_output = tensor_output[tensor_iterator];
        	assert(my_output <= 1.0f);
        	output_buffer[tensor_iterator] = my_output;
        }
    }

}

void
inference_predict() {
	const auto tflite_status =
			s_interpreter->Invoke();
	assert(tflite_status == kTfLiteOk);
}


