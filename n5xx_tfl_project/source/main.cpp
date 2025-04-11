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
#include "app/app.h"

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
    /* Init board hardware. */
	BOARD_InitHardware();

	app_setup();
	app_main_loop();

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
