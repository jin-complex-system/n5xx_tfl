#ifndef SD_CARD_H
#define SD_CARD_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Setup the SD card
 *
 * @return true if successful; false otherwise
 */
bool
sd_card_setup(void);

/**
 * Create directory inside SD card
 *
 * @param directory_str string containing the directory filename
 * @param directory_str_length non-zero length of string
 * @param exists_ok if true, return false if the directory already exists
 * @return true if successful
 */
bool
sd_card_create_directory(
		const char* directory_str,
		const uint16_t directory_str_length,
		const bool exists_ok);

/**
 * Write to SD card as file
 *
 * @param filename_str string containing the directory filename
 * @param filename_str_length non-zero length of filename_str
 * @param directory_str string containing the directory
 * @param directory_string_length non-zero length of string
 * @param data_buffer buffer containing the data to be written into
 * @param data_buffer_length length of the data buffer
 * @param overwrite if true, ignore if the file exist
 *
 * @return true if successful
 */
bool
sd_card_write_to_file(
		const char* filename_str,
		const uint16_t filename_str_length,
		const char* directory_str,
		const uint16_t directory_str_length,
		const void* data_buffer,
		const uint16_t data_buffer_length,
		const bool overwrite);

#ifdef __cplusplus
}
#endif

#endif /* SD_CARD_H */
