/*
 * fan.c
 *
 *  Created on: 26 juin 2019
 *      Author: Nirgal
 */

#include "config.h"
#include "stm32g4_gpio.h"
#include "fan.h"

#define FAN_GPIO				GPIOB
#define FAN_PIN					GPIO_PIN_4

void FAN_init(void)
{
	//Initialisation du port du ventilateur en sortie Push-Pull
	BSP_GPIO_pin_config(FAN_GPIO, FAN_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH, GPIO_NO_AF);
	FAN_set(false);
}


void FAN_set(bool enable)
{
	HAL_GPIO_WritePin(FAN_GPIO, FAN_PIN, enable);
}
