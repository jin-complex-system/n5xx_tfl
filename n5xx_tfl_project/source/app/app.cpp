#include "app.h"

#include <led/led.h>
#include <button/button.h>
#include <cassert>
#include <fsl_debug_console.h>
#include <sd_card/sd_card.h>
#include <cstring>
#include <string>
#include <cr_section_macros.h>
#include <inference/inference_definitions.h>
#include <inference/inference.h>

__BSS(RAM3)
static APP_STATE
current_state = APP_STATE_INIT;

static constexpr
uint16_t
MAX_NUM_DATA = 110u;

__BSS(RAM3)
static
uint16_t
num_iterations = 0;

__BSS(RAM3)
static
inference_input_data_type
input_buffer[INFERENCE_INPUT_ARRAY_LENGTH];

__BSS(RAM3)
static
uint32_t
input_buffer_length = 0;

__BSS(RAM3)
static
inference_output_data_type
OUTPUT_BUFFER[NUM_CLASSES] = {};

/// SD card stuff
const
std::string
INPUT_DIRECTORY = "INPUT";
const
std::string
OUTPUT_FILENAME = "RESULTS.TXT";
const
std::string
OUTPUT_DIRECTORY = "OUTPUT";
constexpr
bool
OVERWRITE_FILE_IS_OKAY = true;

FILINFO
current_file_info;

static constexpr
uint32_t
MAX_INPUT_LENGTH = sizeof(current_file_info.fname);

static constexpr
uint32_t
EXPECTED_INPUT_FILENAME_LENGTH = MAX_INPUT_LENGTH + 1;

static const
uint32_t
MAX_INPUT_FILEPATH_LENGTH = INPUT_DIRECTORY.length() + 1 + EXPECTED_INPUT_FILENAME_LENGTH;

static constexpr
uint32_t
NUM_CHARS_PER_LINE = EXPECTED_INPUT_FILENAME_LENGTH + 20;

__BSS(RAM3)
static
std::string
str_buffer(MAX_NUM_DATA * NUM_CHARS_PER_LINE, 0);

__BSS(RAM3)
uint8_t
best_scores[MAX_NUM_DATA];

__BSS(RAM3)
uint8_t
best_class_ids[MAX_NUM_DATA];

void
app_setup() {
	/// Setup modules
	setup_button();
    setup_led();

    bool success =
	sd_card_setup();
    assert(success);
    PRINTF("Initialised SD card\r\n");

    toggle_green_led();

    inference_setup();
    toggle_blue_led();

    PRINTF("Hello World\r\n");
	current_state = APP_STATE_INIT;
}

void
app_main_loop() {
	bool
	sd_card_loop = false;
	bool
	success;

	/// Store results
	uint8_t
	best_score = 0;
	uint8_t
	best_class_id = 0;

	while(true) {
		switch(current_state) {
		case APP_STATE_CHECK_BUTTON:
		{
	    	if (check_button_result() > 0) {
	    		num_iterations = 0;

	    		// TODO: Move SD card insertion checking here

	            /// Open input directory
	            success = sd_card_open_directory(
	        		INPUT_DIRECTORY.data(),
	        		INPUT_DIRECTORY.length()
	        	);
	            assert(success);

	            sd_card_loop = true;
	    		toggle_blue_led();
	    	}

	    	if (sd_card_loop) {
	    		current_state = APP_STATE_READ_SD_CARD;
	    	}
	    	else {
	    		current_state = APP_STATE_SET_DEFAULT_BUFFER;
	    	}
			break;
		}

		case APP_STATE_SET_DEFAULT_BUFFER:
		{
	    	/// Default buffer is just all zeros
	    	memset(input_buffer, 0, sizeof(input_buffer));

	    	current_state = APP_STATE_INFERENCE;
			break;
		}

		case APP_STATE_READ_SD_CARD:
		{
			assert(sd_card_loop);

			success = sd_card_get_next_file_information(&current_file_info);
			if (!success || num_iterations >= MAX_NUM_DATA) {
				sd_card_close_directory();
				current_state = APP_STATE_SAVE_SD_CARD;
			}
			else {
				/// Craft filepath to file
				std::string filepath(MAX_INPUT_FILEPATH_LENGTH, 0);
				sprintf(
					filepath.data(),
					"%s/%s",
					INPUT_DIRECTORY.data(),
					current_file_info.fname);
				PRINTF("Read input file: %s\r\n", filepath.data());

				/// Read data into input buffer
				input_buffer_length = sd_card_read_from_file(
						input_buffer,
						sizeof(input_buffer),
						filepath.data()
				);
				assert(
						input_buffer_length > 0 &&
						input_buffer_length <= INFERENCE_INPUT_ARRAY_LENGTH * sizeof(inference_input_data_type));

				/// Copy over filename to str_buffer
	    		char* new_line_ptr = str_buffer.data() + num_iterations * NUM_CHARS_PER_LINE;
				sprintf(
					new_line_ptr,
					"%s",
					current_file_info.fname);
				current_state = APP_STATE_INFERENCE;
			}
			break;
		}
		case APP_STATE_SAVE_SD_CARD:
		{
			assert(sd_card_loop);

		    success =
		    sd_card_create_directory(
					OUTPUT_DIRECTORY.data(),
					OUTPUT_DIRECTORY.length(),
					true
			);
		    assert(success);
		    PRINTF("Created output directory\r\n");

	    	/// Store formatted results into a buffer
	    	/// TODO: Add filename/ID as part of results
	    	for (uint32_t iterator = 0; iterator < num_iterations; iterator++) {
	    		char* new_line_ptr = str_buffer.data() + iterator * NUM_CHARS_PER_LINE + EXPECTED_INPUT_FILENAME_LENGTH;

				sprintf(
						new_line_ptr,
						" id=%u score=%u\r\n",
						best_class_ids[iterator],
						best_scores[iterator]);
	    	}

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
				str_buffer.data(),
				str_buffer.length(),
				OVERWRITE_FILE_IS_OKAY
			);
	    	assert(success);
	    	sd_card_close_directory();

	    	/// Toggle blue LED
	    	toggle_blue_led();

			sd_card_loop = false;
			PRINTF("Done writing to SD card\r\n");
			current_state = APP_STATE_CHECK_BUTTON;
			break;
		}
		case APP_STATE_INFERENCE:
		{
	    	inference_set_input(
	    			input_buffer,
					INFERENCE_INPUT_ARRAY_LENGTH);
	    	inference_predict();
	    	inference_get_output(
	    			OUTPUT_BUFFER,
					NUM_CLASSES);

	    	toggle_red_led();

	    	current_state = APP_STATE_PROCESS_INFERENCE;
			break;
		}
		case APP_STATE_PROCESS_INFERENCE:
		{
			best_score = 0;

			/// Grab the best score from the output buffer
			for (uint32_t iterator = 0; iterator < NUM_CLASSES; iterator++) {
				if (best_score < OUTPUT_BUFFER[iterator]) {
					best_class_id = iterator;
					best_score = OUTPUT_BUFFER[iterator];
				}
			}
			assert(best_score >= 0 && best_class_id < NUM_CLASSES);

			best_class_ids[num_iterations] = best_class_id;
			best_scores[num_iterations] = best_score;

			PRINTF(
					"id=%u score=%u\r\n",
					best_class_id,
					best_score);

			if (sd_card_loop) {
				num_iterations++;
			}

			current_state = APP_STATE_CHECK_BUTTON;
			break;
		}
		case APP_STATE_INIT:
		default:
			current_state = APP_STATE_CHECK_BUTTON;
			break;
		}
	}
}

