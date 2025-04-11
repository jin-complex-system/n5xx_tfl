#ifndef APP_H_
#define APP_H_


typedef enum {
	APP_STATE_INIT = 0,         // initial state
	APP_STATE_CHECK_BUTTON,
	APP_STATE_SET_DEFAULT_BUFFER,
	APP_STATE_READ_SD_CARD,
	APP_STATE_SAVE_SD_CARD,
	APP_STATE_INFERENCE,
	APP_STATE_PROCESS_INFERENCE,
} APP_STATE;

void
app_setup();

/**
 * Main application loop
 */
void
app_main_loop();


#endif /* APP_H_ */
