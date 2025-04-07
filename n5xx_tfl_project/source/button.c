#include "button.h"

#include "board.h"
#include <assert.h>
#include "fsl_gpio.h"
#include "pin_mux.h"

#define BOARD_SW_GPIO        BOARD_SW3_GPIO
#define BOARD_SW_PORT        BOARD_SW3_PORT
#define BOARD_SW_GPIO_PIN    BOARD_SW3_GPIO_PIN
#define BOARD_SW_IRQ_HANDLER BOARD_SW3_IRQ_HANDLER
#define BOARD_SW_IRQ         BOARD_SW3_IRQ
#define BOARD_SW_NAME        BOARD_SW3_NAME

static const
gpio_pin_config_t sw_config = {
	kGPIO_DigitalInput,
	0,
};

static volatile
uint8_t
button_result = 0;

void
setup_button(void) {
	BOARD_InitBUTTONsPins();

	CLOCK_EnableClock(kCLOCK_Gpio0);

	GPIO_SetPinInterruptConfig(
			BOARD_SW2_GPIO,
			BOARD_SW2_GPIO_PIN,
			kGPIO_InterruptStatusFlagDisabled
	);
	GPIO_SetPinInterruptConfig(
			BOARD_SW4_GPIO,
			BOARD_SW4_GPIO_PIN,
			kGPIO_InterruptStatusFlagDisabled
	);
	GPIO_SetPinInterruptConfig(
			BOARD_SW_GPIO,
			BOARD_SW_GPIO_PIN,
			kGPIO_InterruptFallingEdge);

	DisableIRQ(BOARD_SW2_IRQ);
	DisableIRQ(BOARD_SW4_IRQ);
	status_t status = EnableIRQ(BOARD_SW_IRQ);
	assert(status == kStatus_Success);

	GPIO_PinInit(
			BOARD_SW_GPIO,
			BOARD_SW_GPIO_PIN,
			&sw_config);
}

const
uint8_t
check_button_result(void) {
	uint8_t result = button_result;
	button_result = 0;

	return result;
}

/// Note that ISR must be place in a C ISR
/// See https://community.nxp.com/t5/MCUXpresso-IDE/ISR-only-recognized-if-placed-in-a-c-source-file-cpp-housed-ISR/td-p/716093
void
BOARD_SW_IRQ_HANDLER(void) {
	GPIO_GpioClearInterruptFlags(
			BOARD_SW_GPIO,
			1U << BOARD_SW_GPIO_PIN);

	button_result++;

	SDK_ISR_EXIT_BARRIER;
}
