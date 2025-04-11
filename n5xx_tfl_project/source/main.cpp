/*
 * Copyright 2016-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    n5xx_tfl_project.cpp
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

#include <cassert>
#include "led.h"
#include "inference.h"
#include "input.h"
#include "button.h"

#include <sd_card/sd_card.h>
#include <cstring>
#include <string>

static constexpr
uint32_t
NUM_CLASSES = 10;

static constexpr
uint16_t
MAX_NUM_DATA = 200u;

static
uint16_t
num_iterations = 0;

static
uint8_t
input_buffer[INPUT_ARRAY_LENGTH];

static
uint32_t
input_buffer_length = 0;

static
float
OUTPUT_BUFFER[NUM_CLASSES] = {};

const std::string
INPUT_DIRECTORY = "INPUT";

const std::string
OUTPUT_FILENAME = "RESULTS.TXT";
const std::string
OUTPUT_DIRECTORY = "OUTPUT";
constexpr bool
OVERWRITE_FILE_IS_OKAY = true;

static
void BOARD_InitHardware(void) {
	/* attach FRO 12M to FLEXCOMM4 (debug console) */
	CLOCK_SetClkDiv(kCLOCK_DivFlexcom4Clk, 1u);
	CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

	/* attach FRO HF to USDHC */
	CLOCK_SetClkDiv(kCLOCK_DivUSdhcClk, 1u);
	CLOCK_AttachClk(kFRO_HF_to_USDHC);

	CLOCK_EnableClock(kCLOCK_Gpio0);
	CLOCK_EnableClock(kCLOCK_Gpio2);
	CLOCK_EnableClock(kCLOCK_Gpio3);

	// TODO: Add SD card related pin configuration

	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();

#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif

	BOARD_InitBUTTONsPins();
	BOARD_InitLEDsPins();
}

/*
 * @brief   Application entry point.
 */
int main(void) {
	 bool success;

    /* Init board hardware. */
	BOARD_InitHardware();

	/// Setup modules
	setup_button();
    setup_led();

    success =
	sd_card_setup();
    assert(success);
    PRINTF("Initialised SD card\r\n");

    success =
    sd_card_create_directory(
			OUTPUT_DIRECTORY.data(),
			OUTPUT_DIRECTORY.length(),
			true
	);
    assert(success);
    PRINTF("Created directory\r\n");

    toggle_green_led();

    inference_setup();
    toggle_blue_led();

    PRINTF("Hello World\r\n");

    /// Process input and store into string
    uint8_t best_scores_percentage[MAX_NUM_DATA];
    uint8_t best_class_ids[MAX_NUM_DATA];
    FILINFO current_file_info;

    while(num_iterations < MAX_NUM_DATA) {
        /// Open input directory
        success = sd_card_open_directory(
    		INPUT_DIRECTORY.data(),
    		INPUT_DIRECTORY.length()
    	);
        assert(success);

    	success = sd_card_get_next_file_information(&current_file_info);

    	/// Reached the end
    	if (!success) {
    		break;
    	}
    	///  TODO: Load input buffer from filename
    	/// Craft filepath
    	std::string filepath(100, 0);
    	sprintf(
			filepath.data(),
			"%s/%s",
			INPUT_DIRECTORY.data(),
			current_file_info.fname);

    	/// Clear the buffer before reading
    	memset(input_buffer, 0, sizeof(input_buffer));

		input_buffer_length = sd_card_read_from_file(
			input_buffer,
			INPUT_ARRAY_LENGTH, // TODO: Change this to not rely on hard-coded values
			filepath.data()
		);
    	assert(input_buffer_length > 0 && input_buffer_length < INPUT_ARRAY_LENGTH);
    	// assert(input_buffer_length == INPUT_ARRAY_LENGTH); // TODO: Uncomment this line when there is actual data

    	sd_card_close_directory();

    	/// Handle button
    	if (check_button_result() > 0) {
    		toggle_blue_led();
    	}

    	/// Get prediction
    	inference_set_input(
    			INPUT_ARRAY,
				INPUT_ARRAY_LENGTH);
    	inference_predict();
    	inference_get_output(
    			OUTPUT_BUFFER,
				NUM_CLASSES);

    	float best_score = -1.0f;
    	uint8_t best_class_id = NUM_CLASSES + 1;

    	/// Grab the best score from the output buffer
    	for (uint32_t iterator = 0; iterator < NUM_CLASSES; iterator++) {
    		if (best_score < OUTPUT_BUFFER[iterator]) {
    			best_class_id = iterator;
    			best_score = OUTPUT_BUFFER[iterator];
    		}
    	}
    	assert(best_score >= 0.0f && best_class_id < NUM_CLASSES);

    	/// Float is unsupported in sprintf
    	/// For percentage, we only care the 2 digits
    	best_scores_percentage[num_iterations] = (uint8_t)(best_score * 100.0f);
    	best_class_ids[num_iterations] = best_class_id;
    	// num_iterations++;

    	/// Store results into a buffer
    	std::string output_buffer(100, 0);
		sprintf(
				output_buffer.data(),
				"id=%u score=%u\r\n",
				best_class_ids[num_iterations],
				best_scores_percentage[num_iterations]);
    	PRINTF("%s\r\n", output_buffer.data());

    	// TODO: Nicely format data before writing to file

    	success =
		sd_card_open_directory(
			OUTPUT_DIRECTORY.data(),
			OUTPUT_DIRECTORY.length()
		);
    	assert(success);

    	/// Export results to SD card
    	success =
    	sd_card_write_to_file(
			OUTPUT_FILENAME.data(),
			OUTPUT_FILENAME.length(),
			OUTPUT_DIRECTORY.data(),
			OUTPUT_DIRECTORY.length(),
			output_buffer.data(),
			output_buffer.length(),
			OVERWRITE_FILE_IS_OKAY
		);
    	assert(success);
    	sd_card_close_directory();

    	/// Toggle red LED
    	toggle_red_led();
    }

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
    }
    return 0 ;
}
