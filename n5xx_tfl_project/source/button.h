#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Setup button
 */
void
setup_button(void);

/**
 * Return the state of button.
 */
const
uint8_t
check_button_result(void);

#if defined(__cplusplus)
}
#endif

#endif //BUTTON_H
