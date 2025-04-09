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

float
OUTPUT_BUFFER[NUM_CLASSES] = {};

const std::string
OUTPUT_FILENAME = "my_results.txt";
const std::string
OUTPUT_DIRECTORY = "my_results_dir";
constexpr bool
OVERWRITE_FILE_IS_OKAY = true;

/*
 * @brief   Application entry point.
 */
int main(void) {
    setup_button();

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    setup_led();

    const bool sd_card_success =
	sd_card_setup();
    assert(sd_card_success);

    toggle_green_led();

    inference_setup();
    toggle_blue_led();

    PRINTF("Hello World\r\n");

    while(true) {
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
    	uint32_t best_class_id = NUM_CLASSES + 1;

    	/// Grab the best score from the output buffer
    	for (uint32_t iterator = 0; iterator < NUM_CLASSES; iterator++) {
    		if (best_score < OUTPUT_BUFFER[iterator]) {
    			best_class_id = iterator;
    			best_score = OUTPUT_BUFFER[iterator];
    		}

    	}
    	assert(best_score >= 0.0f and best_class_id < NUM_CLASSES);

    	/// Store results into a buffer
    	std::string output_buffer(100, 0);
		sprintf(
				output_buffer.data(),
				"id=%lu score=%.5f\r\n",
				best_class_id,
				best_score);
		output_buffer.shrink_to_fit();
    	PRINTF("%s\r\n", output_buffer.data());

    	/// Export results to SD card
    	const bool sd_result =
    	sd_card_write_to_file(
			OUTPUT_FILENAME.data(),
			OUTPUT_FILENAME.length(),
			OUTPUT_DIRECTORY.data(),
			OUTPUT_DIRECTORY.length(),
			output_buffer.data(),
			output_buffer.length(),
			OVERWRITE_FILE_IS_OKAY
		);
    	assert(sd_result);

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
