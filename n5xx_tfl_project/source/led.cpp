#include "led.h"

#include <pin_mux.h>
#include <board.h>

void
setup_led() {
	BOARD_InitLEDsPins();
	CLOCK_EnableClock(kCLOCK_Gpio3);

    LED_RED_INIT(LOGIC_LED_OFF);
    LED_GREEN_INIT(LOGIC_LED_OFF);
    LED_BLUE_INIT(LOGIC_LED_OFF);
}

void
toggle_red_led() {
	LED_RED_TOGGLE();
}

void
toggle_green_led() {
	LED_GREEN_TOGGLE();
}

void
toggle_blue_led() {
	LED_BLUE_TOGGLE();
}
