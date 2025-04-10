#include "sd_card.h"

#include <assert.h>
#include <string.h>
#include <fsl_sd.h>
#include <fsl_common.h>
#include <fsl_port.h>
#include <pin_mux.h>
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
void
setup_sd_card_pins(void) {
	/* Enables the clock for PORT1: Enables clock */
	CLOCK_EnableClock(kCLOCK_Port1);
	/* Enables the clock for PORT2: Enables clock */
	CLOCK_EnableClock(kCLOCK_Port2);

	const port_pin_config_t port1_8_pinA1_config = {/* Internal pull-up/down resistor is disabled */
													kPORT_PullDisable,
													/* Low internal pull resistor value is selected. */
													kPORT_LowPullResistor,
													/* Fast slew rate is configured */
													kPORT_FastSlewRate,
													/* Passive input filter is disabled */
													kPORT_PassiveFilterDisable,
													/* Open drain output is disabled */
													kPORT_OpenDrainDisable,
													/* Low drive strength is configured */
													kPORT_LowDriveStrength,
													/* Pin is configured as FC4_P0 */
													kPORT_MuxAlt2,
													/* Digital input enabled */
													kPORT_InputBufferEnable,
													/* Digital input is not inverted */
													kPORT_InputNormal,
													/* Pin Control Register fields [15:0] are not locked */
													kPORT_UnlockRegister};
	/* PORT1_8 (pin A1) is configured as FC4_P0 */
	PORT_SetPinConfig(PORT1, 8U, &port1_8_pinA1_config);

	const port_pin_config_t port1_9_pinB1_config = {/* Internal pull-up/down resistor is disabled */
													kPORT_PullDisable,
													/* Low internal pull resistor value is selected. */
													kPORT_LowPullResistor,
													/* Fast slew rate is configured */
													kPORT_FastSlewRate,
													/* Passive input filter is disabled */
													kPORT_PassiveFilterDisable,
													/* Open drain output is disabled */
													kPORT_OpenDrainDisable,
													/* Low drive strength is configured */
													kPORT_LowDriveStrength,
													/* Pin is configured as FC4_P1 */
													kPORT_MuxAlt2,
													/* Digital input enabled */
													kPORT_InputBufferEnable,
													/* Digital input is not inverted */
													kPORT_InputNormal,
													/* Pin Control Register fields [15:0] are not locked */
													kPORT_UnlockRegister};
	/* PORT1_9 (pin B1) is configured as FC4_P1 */
	PORT_SetPinConfig(PORT1, 9U, &port1_9_pinB1_config);

	const port_pin_config_t port2_1_pinH1_config = {/* Internal pull-up/down resistor is disabled */
													kPORT_PullDisable,
													/* Low internal pull resistor value is selected. */
													kPORT_LowPullResistor,
													/* Fast slew rate is configured */
													kPORT_FastSlewRate,
													/* Passive input filter is disabled */
													kPORT_PassiveFilterDisable,
													/* Open drain output is disabled */
													kPORT_OpenDrainDisable,
													/* Low drive strength is configured */
													kPORT_LowDriveStrength,
													/* Pin is configured as PIO2_1 */
													kPORT_MuxAlt0,
													/* Digital input enabled */
													kPORT_InputBufferEnable,
													/* Digital input is not inverted */
													kPORT_InputNormal,
													/* Pin Control Register fields [15:0] are not locked */
													kPORT_UnlockRegister};
	/* PORT2_1 (pin H1) is configured as PIO2_1 */
	PORT_SetPinConfig(PORT2, 1U, &port2_1_pinH1_config);

	const port_pin_config_t port2_2_pinH3_config = {/* Internal pull-up resistor is enabled */
													kPORT_PullUp,
													/* Low internal pull resistor value is selected. */
													kPORT_LowPullResistor,
													/* Fast slew rate is configured */
													kPORT_FastSlewRate,
													/* Passive input filter is disabled */
													kPORT_PassiveFilterDisable,
													/* Open drain output is disabled */
													kPORT_OpenDrainDisable,
													/* Low drive strength is configured */
													kPORT_LowDriveStrength,
													/* Pin is configured as SDHC0_D1 */
													kPORT_MuxAlt3,
													/* Digital input enabled */
													kPORT_InputBufferEnable,
													/* Digital input is not inverted */
													kPORT_InputNormal,
													/* Pin Control Register fields [15:0] are not locked */
													kPORT_UnlockRegister};
	/* PORT2_2 (pin H3) is configured as SDHC0_D1 */
	PORT_SetPinConfig(PORT2, 2U, &port2_2_pinH3_config);

	const port_pin_config_t port2_3_pinJ3_config = {/* Internal pull-up resistor is enabled */
													kPORT_PullUp,
													/* Low internal pull resistor value is selected. */
													kPORT_LowPullResistor,
													/* Fast slew rate is configured */
													kPORT_FastSlewRate,
													/* Passive input filter is disabled */
													kPORT_PassiveFilterDisable,
													/* Open drain output is disabled */
													kPORT_OpenDrainDisable,
													/* Low drive strength is configured */
													kPORT_LowDriveStrength,
													/* Pin is configured as SDHC0_D0 */
													kPORT_MuxAlt3,
													/* Digital input enabled */
													kPORT_InputBufferEnable,
													/* Digital input is not inverted */
													kPORT_InputNormal,
													/* Pin Control Register fields [15:0] are not locked */
													kPORT_UnlockRegister};
	/* PORT2_3 (pin J3) is configured as SDHC0_D0 */
	PORT_SetPinConfig(PORT2, 3U, &port2_3_pinJ3_config);

	const port_pin_config_t port2_4_pinK3_config = {/* Internal pull-up/down resistor is disabled */
													kPORT_PullDisable,
													/* Low internal pull resistor value is selected. */
													kPORT_LowPullResistor,
													/* Fast slew rate is configured */
													kPORT_FastSlewRate,
													/* Passive input filter is disabled */
													kPORT_PassiveFilterDisable,
													/* Open drain output is disabled */
													kPORT_OpenDrainDisable,
													/* Low drive strength is configured */
													kPORT_LowDriveStrength,
													/* Pin is configured as SDHC0_CLK */
													kPORT_MuxAlt3,
													/* Digital input enabled */
													kPORT_InputBufferEnable,
													/* Digital input is not inverted */
													kPORT_InputNormal,
													/* Pin Control Register fields [15:0] are not locked */
													kPORT_UnlockRegister};
	/* PORT2_4 (pin K3) is configured as SDHC0_CLK */
	PORT_SetPinConfig(PORT2, 4U, &port2_4_pinK3_config);

	const port_pin_config_t port2_5_pinK1_config = {/* Internal pull-up resistor is enabled */
													kPORT_PullUp,
													/* Low internal pull resistor value is selected. */
													kPORT_LowPullResistor,
													/* Fast slew rate is configured */
													kPORT_FastSlewRate,
													/* Passive input filter is disabled */
													kPORT_PassiveFilterDisable,
													/* Open drain output is disabled */
													kPORT_OpenDrainDisable,
													/* Low drive strength is configured */
													kPORT_LowDriveStrength,
													/* Pin is configured as SDHC0_CMD */
													kPORT_MuxAlt3,
													/* Digital input enabled */
													kPORT_InputBufferEnable,
													/* Digital input is not inverted */
													kPORT_InputNormal,
													/* Pin Control Register fields [15:0] are not locked */
													kPORT_UnlockRegister};
	/* PORT2_5 (pin K1) is configured as SDHC0_CMD */
	PORT_SetPinConfig(PORT2, 5U, &port2_5_pinK1_config);

	const port_pin_config_t port2_6_pinK2_config = {/* Internal pull-up resistor is enabled */
													kPORT_PullUp,
													/* Low internal pull resistor value is selected. */
													kPORT_LowPullResistor,
													/* Fast slew rate is configured */
													kPORT_FastSlewRate,
													/* Passive input filter is disabled */
													kPORT_PassiveFilterDisable,
													/* Open drain output is disabled */
													kPORT_OpenDrainDisable,
													/* Low drive strength is configured */
													kPORT_LowDriveStrength,
													/* Pin is configured as SDHC0_D3 */
													kPORT_MuxAlt3,
													/* Digital input enabled */
													kPORT_InputBufferEnable,
													/* Digital input is not inverted */
													kPORT_InputNormal,
													/* Pin Control Register fields [15:0] are not locked */
													kPORT_UnlockRegister};
	/* PORT2_6 (pin K2) is configured as SDHC0_D3 */
	PORT_SetPinConfig(PORT2, 6U, &port2_6_pinK2_config);

	const port_pin_config_t port2_7_pinL2_config = {/* Internal pull-up resistor is enabled */
													kPORT_PullUp,
													/* Low internal pull resistor value is selected. */
													kPORT_LowPullResistor,
													/* Fast slew rate is configured */
													kPORT_FastSlewRate,
													/* Passive input filter is disabled */
													kPORT_PassiveFilterDisable,
													/* Open drain output is disabled */
													kPORT_OpenDrainDisable,
													/* Low drive strength is configured */
													kPORT_LowDriveStrength,
													/* Pin is configured as SDHC0_D2 */
													kPORT_MuxAlt3,
													/* Digital input enabled */
													kPORT_InputBufferEnable,
													/* Digital input is not inverted */
													kPORT_InputNormal,
													/* Pin Control Register fields [15:0] are not locked */
													kPORT_UnlockRegister};
	/* PORT2_7 (pin L2) is configured as SDHC0_D2 */
	PORT_SetPinConfig(PORT2, 7U, &port2_7_pinL2_config);
}

static
status_t
sdcardWaitCardInsert(void) {
	sd_card_initialise = false;

	setup_sd_card_pins();

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
			_T("/dir_1/f_1.txt"),
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

    /// Flush the buffer and close
    error = f_sync(&g_fileObject);
    if (error) {
        PRINTF("Flushing file failed. \r\n");
        return false;
    }

    error = f_close(&g_fileObject);
    if (error) {
        PRINTF("Closing the file failed. \r\n");
        return false;
    }

    return true;
}

