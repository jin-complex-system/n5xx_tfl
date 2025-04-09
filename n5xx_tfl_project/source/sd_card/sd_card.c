#include "sd_card.h"

#include <assert.h>
#include <string.h>
#include <fsl_sd.h>
#include <fsl_debug_console.h>
#include <ff.h>
#include <diskio.h>
#include <fsl_sd_disk.h>
#include "sdmmc_config.h"
#include <board.h>

static
status_t
sdcardWaitCardInsert(void);

static FATFS g_fileSystem; /* File system object */
static FIL g_fileObject;   /* File object */

static
bool
sd_card_initialise = false;

static
status_t
sdcardWaitCardInsert(void) {
	sd_card_initialise = false;

    BOARD_SD_Config(
    		&g_sd,
			NULL,
			BOARD_SDMMC_SD_HOST_IRQ_PRIORITY,
			NULL);

    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success) {
        PRINTF("\r\nSD host init fail\r\n");
        return kStatus_Fail;
    }

    /* wait card insert */
    if (SD_PollingCardInsert(&g_sd, kSD_Inserted) == kStatus_Success) {
        PRINTF("\r\nCard inserted.\r\n");

        /* power off card */
        SD_SetCardPower(&g_sd, false);
        /* power on the card */
        SD_SetCardPower(&g_sd, true);
    }
    else {
        PRINTF("\r\nCard detect fail.\r\n");
        return kStatus_Fail;
    }

    sd_card_initialise = true;
    return kStatus_Success;
}

bool
sd_card_setup(void) {
	const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
	FRESULT error;

	if (sdcardWaitCardInsert() != kStatus_Success) {
		if (!SD_IsCardPresent(&g_sd)) {
			return false;
		}
	}

    if (f_mount(&g_fileSystem, driverNumberBuffer, 0U)) {
        PRINTF("Mount volume failed.\r\n");
        return false;
    }

#if (FF_FS_RPATH >= 2U)
    error = f_chdrive((char const *)&driverNumberBuffer[0U]);
    if (error) {
        PRINTF("Change drive failed.\r\n");
        return false;
    }
#endif

#if FF_USE_MKFS
	BYTE work[FF_MAX_SS];

    PRINTF("\r\nMake file system......The time may be long if the card capacity is big.\r\n");
    if (f_mkfs(driverNumberBuffer, 0, work, sizeof work)) {
        PRINTF("Make file system failed.\r\n");
        return false;
    }
#endif /* FF_USE_MKFS */

    return true;
}

bool
sd_card_create_directory(
		const char* directory_str,
		const uint16_t directory_str_length,
		const bool exists_ok) {
	FRESULT error;

	/// Check parameters
	assert(sd_card_initialise);
	assert(directory_str != NULL);
	assert(directory_str_length > 0);

	// TODO: Create directory based on parameters

    error = f_mkdir(_T("/dir_1"));
    if (error) {
        if (error == FR_EXIST) {
            PRINTF("Directory exists.\r\n");
            return exists_ok;
        }
        else {
            PRINTF("Make directory failed.\r\n");
            return false;
        }
    }
    return true;
}

bool
sd_card_write_to_file(
		const char* filename_str,
		const uint16_t filename_str_length,
		const char* directory_str,
		const uint16_t directory_str_length,
		const void* data_buffer,
		const uint16_t data_buffer_length,
		const bool overwrite) {
	FRESULT error;
	DIR directory; /* Directory object */

	/// Check parameters
	assert(sd_card_initialise);
	assert(filename_str != NULL);
	assert(filename_str_length > 0);
	assert(directory_str != NULL);
	assert(directory_str_length > 0);

	// TODO: Open directory based on parameters

    if (f_opendir(&directory, "/dir_1")) {
        PRINTF("Open directory failed.\r\n");
        return false;
    }
    error = f_open(
    		&g_fileObject,
			_T("/dir_1/f_1.dat"),
			(FA_WRITE | FA_READ | FA_CREATE_ALWAYS));
    if (error) {
        if (error == FR_EXIST) {
            if (overwrite) {
            	PRINTF("File exists. Exiting\r\n");
            	return false;
            }
            else {
            	PRINTF("Overwriting existing file\r\n");
            }
        }
        else {
            PRINTF("Open file failed.\r\n");
            return false;
        }
    }

    /// Write to the file in one shot
    UINT bytesWritten = 0;
    error = f_write(
    		&g_fileObject,
			data_buffer,
			data_buffer_length,
			&bytesWritten);
    if ((error) || (bytesWritten != data_buffer_length)) {
        PRINTF("Write file failed. \r\n");
        return false;
    }

    return true;
}

