/*
 * button.h
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "stm32g4_utils.h"

typedef enum
{
	BUTTON_NO_EVENT,
	BUTTON_PRESS_EVENT,
	BUTTON_RELEASE_EVENT
}button_e;

void BUTTONS_init(void);
void BUTTONS_update(button_e * button_left_event, button_e * button_right_event, button_e * button_up_event, button_e * button_down_event, button_e * button_center_event);

uint8_t BUTTON_left_read(void);
uint8_t BUTTON_right_read(void);
uint8_t BUTTON_up_read(void);
uint8_t BUTTON_down_read(void);
uint8_t BUTTON_center_read(void);

#endif /* BUTTON_H_ */
