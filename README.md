# n5xx_tfl

This project is intended to test the conversion step of TensorFlow models on the evaluation board `MCX-N5XX-EVK`. It also uses LEDs to have a quick way to measure the inference speed.

# Requirements

## Hardware Requirements
- `MCX-N5XX-EVK`

## Software Requirements
- MCUXpresso IDE
- Your target model converted from eIQ portal and eIQ Converter

# Setup

## Reproduce Project Configuration
1. Open a new C/C++ Project
2. Create a new project targeting MCXN547 evaluation board and click 'Next'
3. Rename the project name and configure your desired location
4. Set the following project properties:
    - Project Type: C++ Project
    - Project Options - SDK Debug: UART
5. Add the following components:
   - Abstraction > Device > SDK Drivers > gpio_adapter
   - Middleware > Machine Learning > eIQ
   - Middleware > Machine Learning > tensorflow_lite
   - Middleware > File System > FAT File System > sd
   - Middleware > Memories > SDMMC Stack > mmc
   - Middleware > Memories > SDMMC Stack > sd
   - Middleware > Memories > SDMMC Stack > sdio
   - Middleware > Memories > SDMMC Stack > sdmmc_host_usdhc
   - Middleware > Memories > SDMMC Stack > sdmmc_host_usdhc_non_blocking
   - Middleware > Memories > SDMMC Stack > sdmmc_osa
7. Remove the following components:
   - Middleware > Memories > SDMMC Stack > sdmmc_host_usdhc_blocking
6. Click on 'Finish'
7. Copy over source files to `<project_name>/source`
8. Copy over files from [`_copy_to_locations`](copy_to_locations) to various driver locations:
   - Copy [`_copy_to_locations/ffconf.h`](_copy_to_locations/ffconf.h) into `fatfs/source/`
   - Copy [`_copy_to_locations/ffconf_gen.h`](`_copy_to_locations/ffconf_gen.h`) into `sdmmc/template/usdhc`
9. Modify line `232` in`<project_name>/fatfs/source/ff.h` to expand filenames:
```C
// BYTE	fn[12];			/* SFN (in/out) {body[8],ext[3],status[1]} */
BYTE	fn[25];			/* SFN (in/out) {body[21],ext[3],status[1]} */
```

# How to Test New TensorFlow Lite Models
1. Convert your TensorFlow models (`.h5`) using eIQ Portal and command line
   - ```eiq-converter eiq-converter --plugin eiq-converter-neutron --custom-options "dump-header-file-input" --source CNN_model.tflite --dest converted_model.tflite --target mcxn54x```
2. Copy over the output header file into the `<project_name>/source`
3. Inside `inference.cpp`:
    - Update the code line `s_model = tflite::GetModel(CNN_model_data);`
    - Copy over the relevant operators for TFLite micro from your model header file into `model_GetOpsResolver()`
4. If necessary, update `input.h`
5. Run the application and observe the output buffer

# Special Case: Running Transformers with NPU
At the moment, converting transformers using TensorFlow Lite then targetting NPU does not work; SoftMax layer input always remains unsigned integer despite different efforts.

If there is a need to debug transformers, change the following code to remove the asserts:
1. Navigate to [softmax_common.cpp](n5xx_tfl_project/eiq/tensorflow-lite/tensorflow/lite/micro/kernels/softmax_common.cpp)
2. Comment out line 52:
```C++
  } else {
    // TF_LITE_ENSURE_EQ(context, input->type, output->type);
  }
```
3. Change lines 101 - 109:
```C++
      }
      else if (output->type == kTfLiteUInt8) {
	    TF_LITE_ENSURE_TYPES_EQ(context, output->type, kTfLiteUInt8);
	    TF_LITE_ENSURE_EQ(context, output->params.zero_point, 0);
      }
      else {  // output->type == kTfLiteint8
    	TF_LITE_ENSURE_TYPES_EQ(context, output->type, kTfLiteInt8);
        TF_LITE_ENSURE_EQ(context, output->params.zero_point, -128);
        TF_LITE_ENSURE(context, output->params.scale == 1.f / 256);
```

Note that doing the above will hang the core with running NPU.