/**
 *******************************************************************************
 * @file 	main.c
 * @author 	Nirgal
 * @date 	Mar 29, 2024
 * @brief	Fichier principal de votre projet sur carte Nucleo STM32G431KB
 *******************************************************************************
 */

#include "config.h"
#include "stm32g4_sys.h"

#include "stm32g4_systick.h"
#include "stm32g4_gpio.h"
#include "stm32g4_uart.h"
#include "stm32g4_utils.h"
#include <stdio.h>
#include "maestro.h"
#include "fan.h"
#include "led.h"

void process_test_fan(void);
void process_test_adc(void);
void process_test_led(void);


/**
  * @brief  Point d'entée de votre application
  */
int main(void)
{
	/* Cette ligne doit rester la première de votre main !
	 * Elle permet d'initialiser toutes les couches basses des drivers (Hardware Abstraction Layer),
	 * condition préalable indispensable à l'exécution des lignes suivantes.
	 */
	HAL_Init();

	/* Initialisation des périphériques utiliés dans votre programme */
	BSP_GPIO_enable();
	BSP_UART_init(UART2_ID,115200);

	/* Indique que les printf sont dirigés vers l'UART2 */
	BSP_SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	/* Initialisation du port de la led Verte (carte Nucleo) */
	BSP_GPIO_pin_config(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH,GPIO_NO_AF);

	//process_test_fan();	//fonction blocante ! (� des fins de test)
	//process_test_adc();	//fonction blocante ! (� des fins de test)
	//process_test_led();	//fonction blocante ! (� des fins de test)

	MAESTRO_init();

	while (1)
	{
		MAESTRO_process_main();
	}
}


/*
 * Fonction de test blocante pour valider le pilotage du ventilateur
 * @brief	Le ventilateur s'allume 2 secondes, puis s'éteint 2 secondes, puis recommence.
 */
void process_test_fan(void)
{
	FAN_init();
	while(1)
	{
		FAN_set(TRUE);
		HAL_Delay(2000);
		FAN_set(FALSE);
		HAL_Delay(2000);
	}
}


/*
 * Fonction de test blocante pour valider l'acquisition de la valeur du potentiomètre sur l'ADC
 * @brief	La donnée acquise par l'ADC est envoyée sur la liaison série UART2.
 */
void process_test_adc(void)
{
	int16_t value;
	BSP_ADC_init();
	while(1)
	{
		value = ADC_getValue(ADC_SENSOR_CHANNEL);
		printf("ADC value = %d\n", value);
	}
}


/*
 * Fonction de test blocante pour valider le pilotage de la LED
 * @brief	La led change d'état toutes les 2 secondes (ON --> clignotement --> OFF --> flash).
 */
void process_test_led(void)
{
	LED_init();
	while(1)
	{
		LED_set(LED_ON);
		HAL_Delay(2000);
		LED_set(LED_BLINK);
		HAL_Delay(2000);
		LED_set(LED_OFF);
		HAL_Delay(2000);
		LED_set(LED_FLASH);
		HAL_Delay(2000);
	}
}

