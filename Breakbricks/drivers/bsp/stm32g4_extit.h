/**
 *******************************************************************************
 * @file	stm32g4_extit.h
 * @author	jjo
 * @date	Apr 28, 2024
 * @brief	Gestion des interruptions externes pour cible STM32G4
 *******************************************************************************
 */

#ifndef BSP_STM32G4_EXTIT_H_
#define BSP_STM32G4_EXTIT_H_

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "stm32g4_utils.h"

#if USE_BSP_EXTIT

/* Defines -------------------------------------------------------------------*/


/* Public types --------------------------------------------------------------*/
/**
 * @brief Type pointeur sur fonction de callback pour les interruptions externes
 *
 * @param pin : numéro de la broche qui a généré l'interruption au format GPIO_PIN_x sur l'intervalle [0; 15]
 */
typedef void(*callback_extit_t)(uint16_t pin);

/* Public constants ----------------------------------------------------------*/


/* Public functions declarations ---------------------------------------------*/
void BSP_EXTIT_set_callback(callback_extit_t fun, uint8_t pin_number, bool enable);

void BSP_EXTIT_enable(uint8_t pin_number);

void BSP_EXTIT_disable(uint8_t pin_number);

uint8_t BSP_EXTIT_gpiopin_to_pin_number(uint16_t GPIO_PIN_x);

void BSP_EXTIT_ack_it(uint8_t pin_number);

#endif /* USE_BSP_EXTIT */
#endif /* BSP_STM32G4_EXTIT_H_ */
