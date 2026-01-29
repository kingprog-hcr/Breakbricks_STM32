/*
 * button.c
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */
#include "stm32g4xx_hal.h"
#include "button.h"
#include "stm32g4_systick.h"
#include "stm32g4_gpio.h"

#define GPIO_BUTTON_LEFT GPIOA
#define PIN_BUTTON_LEFT GPIO_PIN_12

#define GPIO_BUTTON_RIGHT GPIOA				//B
#define PIN_BUTTON_RIGHT GPIO_PIN_15		//7

#define GPIO_BUTTON_UP GPIOB				//A
#define PIN_BUTTON_UP GPIO_PIN_7			//15

#define GPIO_BUTTON_DOWN GPIOB
#define PIN_BUTTON_DOWN GPIO_PIN_6

#define GPIO_BUTTON_CENTER GPIOB
#define PIN_BUTTON_CENTER GPIO_PIN_0

void BUTTONS_process_ms(void);
static volatile uint32_t t = 0;

void BUTTONS_init(void)
{
	//Initialisation du port du bouton bleu (carte Nucleo)
	BSP_GPIO_pin_config(GPIO_BUTTON_LEFT, PIN_BUTTON_LEFT, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, GPIO_NO_AF);
	BSP_GPIO_pin_config(GPIO_BUTTON_RIGHT, PIN_BUTTON_RIGHT, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, GPIO_NO_AF);
	BSP_GPIO_pin_config(GPIO_BUTTON_UP, PIN_BUTTON_UP, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, GPIO_NO_AF);
	BSP_GPIO_pin_config(GPIO_BUTTON_DOWN, PIN_BUTTON_DOWN, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, GPIO_NO_AF);
	BSP_GPIO_pin_config(GPIO_BUTTON_CENTER, PIN_BUTTON_CENTER, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, GPIO_NO_AF);
	BSP_systick_add_callback_function(&BUTTONS_process_ms);
}


void BUTTONS_process_ms(void)
{
	if(t)
		t--;
}

bool BUTTON_left_read(void)
{
	return !HAL_GPIO_ReadPin(GPIO_BUTTON_LEFT, PIN_BUTTON_LEFT);
}

bool BUTTON_right_read(void)
{
	return !HAL_GPIO_ReadPin(GPIO_BUTTON_RIGHT, PIN_BUTTON_RIGHT);
}

bool BUTTON_up_read(void)
{
	return !HAL_GPIO_ReadPin(GPIO_BUTTON_UP, PIN_BUTTON_UP);
}

bool BUTTON_down_read(void)
{
	return !HAL_GPIO_ReadPin(GPIO_BUTTON_DOWN, PIN_BUTTON_DOWN);
}

bool BUTTON_center_read(void)
{
	return !HAL_GPIO_ReadPin(GPIO_BUTTON_CENTER, PIN_BUTTON_CENTER);
}

void BUTTONS_update(button_e * button_left_event, button_e * button_right_event, button_e * button_up_event, button_e * button_down_event, button_e * button_center_event)
{
	static bool previous_button_left = false;
	static bool previous_button_right = false;
	static bool previous_button_up = false;
	static bool previous_button_down = false;
	static bool previous_button_center = false;

	*button_left_event = BUTTON_NO_EVENT;
	*button_right_event = BUTTON_NO_EVENT;
	*button_up_event = BUTTON_NO_EVENT;
	*button_down_event = BUTTON_NO_EVENT;
	*button_center_event = BUTTON_NO_EVENT;

	if(!t)
	{
		t = 10;
		bool current_button_left = 	!HAL_GPIO_ReadPin(GPIO_BUTTON_LEFT, 	PIN_BUTTON_LEFT);
		bool current_button_right = 	!HAL_GPIO_ReadPin(GPIO_BUTTON_RIGHT, 	PIN_BUTTON_RIGHT);
		bool current_button_up = 		!HAL_GPIO_ReadPin(GPIO_BUTTON_UP, 		PIN_BUTTON_UP);
		bool current_button_down = 	!HAL_GPIO_ReadPin(GPIO_BUTTON_DOWN, 	PIN_BUTTON_DOWN);
		bool current_button_center = 	!HAL_GPIO_ReadPin(GPIO_BUTTON_CENTER, 	PIN_BUTTON_CENTER);

		if (current_button_left != previous_button_left) {
			*button_left_event = current_button_left ? BUTTON_PRESS_EVENT : BUTTON_RELEASE_EVENT;
			previous_button_left = current_button_left;
		}

		if (current_button_right != previous_button_right) {
			*button_right_event = current_button_right ? BUTTON_PRESS_EVENT : BUTTON_RELEASE_EVENT;
			previous_button_right = current_button_right;
		}

		if (current_button_up != previous_button_up) {
			*button_up_event = current_button_up ? BUTTON_PRESS_EVENT : BUTTON_RELEASE_EVENT;
			previous_button_up = current_button_up;
		}

		if (current_button_down != previous_button_down) {
			*button_down_event = current_button_down ? BUTTON_PRESS_EVENT : BUTTON_RELEASE_EVENT;
			previous_button_down = current_button_down;
		}

		if (current_button_center != previous_button_center) {
			*button_center_event = current_button_center ? BUTTON_PRESS_EVENT : BUTTON_RELEASE_EVENT;
			previous_button_center = current_button_center;
		}
	}
}
