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
   - Middleware > Machine Learning > eIQ
   - Middleware > Machine Learning > tensorflow_lite
6. Click on 'Finish'
7. Copy over source files to <project_name>/source

# How to Test New TensorFlow Lite Models
1. Convert your TensorFlow models (`.h5`) using eIQ Portal and command line
   - ```eiq-converter eiq-converter --plugin eiq-converter-neutron --custom-options "dump-header-file-input" --source CNN_model.tflite --dest converted_model.tflite --target mcxn54x```
2. Copy over the output header file into the `<project_name>/source`
3. Inside `inference.cpp`:
    - Update the code line `s_model = tflite::GetModel(CNN_model_data);`
    - Copy over the relevant operators for TFLite micro from your model header file into `model_GetOpsResolver()`
4. If necessary, update `input.h`
5. Run the application and observe the output buffer
