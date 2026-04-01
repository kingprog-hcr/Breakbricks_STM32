/*
 * maestro.c
 *
 *  Created on: Oct 28, 2024
 *      Author: Nirgal
 */
#include "maestro.h"
#include "button.h"
#include "fan.h"
#include "led.h"
#include "stm32g4_adc.h"
#include "stm32g4_systick.h"
#include <stdio.h>

#define TEMPERATURE_TRESHOLD	2048

static volatile uint32_t t = 0;
static void MAESTRO_state_machine(void);



void MAESTRO_init(void)
{
	BSP_systick_add_callback_function(&MAESTRO_process_ms);
	printf("Welcome in MAESTRO: Machine À États pour la Surveillance Thermique Régulée par Ordinateur\n");
}

void MAESTRO_process_ms(void)
{
	if(t)
		t--;
}

void MAESTRO_process_main(void)
{
	MAESTRO_state_machine();

}

static void MAESTRO_state_machine(void)
{
	typedef enum
	{
		INIT,
		MODE_AUTO,
		MODE_MANUAL_ON,
		MODE_MANUAL_OFF
		//liste des états de la machine
	}state_e;



	button_e button_center_event, button_down_event, button_up_event;
	BUTTONS_update(NULL, NULL, &button_up_event, &button_down_event, &button_center_event);	//A chaque passage ici, on scrute un éventuel évènement sur le bouton
	int16_t adc_value;

	switch(state)
	{
		case INIT:
			BSP_ADC_init();
			BUTTONS_init();
			FAN_init();
			LED_init();

			state = MODE_AUTO;
			break;
		case MODE_AUTO:

			if(entrance){

				printf("mode auto\n");
			}

			adc_value = ADC_getValue(ADC_SENSOR_CHANNEL);

			if(adc_value > TEMPERATURE_TRESHOLD){

			FAN_set(true);
			LED_set(LED_BLINK);

			}
			else{

			FAN_set(false);
			LED_set(LED_FLASH);

			}

			if(button_center_event == BUTTON_PRESS_EVENT){

				state = MODE_MANUAL_ON;
			}
			break;

		case MODE_MANUAL_ON:
			if(entrance){

			printf("mode manuel ON\n");
			FAN_set(true);
			LED_set(LED_ON);

			}

			if(button_center_event == BUTTON_PRESS_EVENT){
				state = MODE_AUTO;
			}

			if (button_down_event == BUTTON_PRESS_EVENT){
				state = MODE_MANUAL_OFF;
			}

			break;
		case MODE_MANUAL_OFF:
			if(entrance){

				printf("mode manuel OFF\n");
				FAN_set(false);
				LED_set(LED_OFF);
			}

			if (button_up_event == BUTTON_PRESS_EVENT){
				state = MODE_MANUAL_ON;
			}

			if (button_center_event == BUTTON_PRESS_EVENT){

				state = MODE_AUTO;
			}

			break;

		default:
			break;
	}
}

