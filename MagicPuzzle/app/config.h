/**
 *******************************************************************************
 * @file 	config.h
 * @author 	jjo
 * @date 	Mar 29, 2024
 * @brief	Fichier principal de configuration de votre projet sur carte Nucléo STM32G431KB.
 * 			Permet d'activer les différents modules logiciels à votre disposition.
 *******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CONFIG_H_
#define CONFIG_H_


/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Defines -------------------------------------------------------------------*/
#define LED_GREEN_PIN 			GPIO_PIN_8
#define LED_GREEN_GPIO 			GPIOB

#define UART2_ON_PA3_PA2
#define UART1_ON_PB7_PB6

#define USE_XPT2046				1

#define USE_ILI9341				1
#define USE_FONT7x10			1
#define USE_FONT11x18			1
#define USE_BSP_TIMER			1
#define USE_BSP_EXTIT           1

#define USE_DHT11				0

#define USE_DS18B20				0

#define USE_BH1750FVI			0

#define USE_BMP180				0

#define USE_YX6300				0
#endif /* CONFIG_H_ */
