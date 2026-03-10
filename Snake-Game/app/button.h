/*
 * button.h
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */

#ifndef BUTTON_H_

#include <stdint.h>
#include "stm32g4_utils.h"
#include "stm32g4xx_hal.h"

#define BUTTON_H_

typedef enum
{
	BUTTON_NO_EVENT,
	BUTTON_PRESS_EVENT,
	BUTTON_RELEASE_EVENT
}button_e;

void BUTTONS_init(void);
void BUTTONS_update(button_e * button_left_event, button_e * button_right_event, button_e * button_up_event, button_e * button_down_event, button_e * button_center_event);

bool BUTTON_left_read(void);
bool BUTTON_right_read(void);
bool BUTTON_up_read(void);
bool BUTTON_down_read(void);
bool BUTTON_center_read(void);

#endif /* BUTTON_H_ */
