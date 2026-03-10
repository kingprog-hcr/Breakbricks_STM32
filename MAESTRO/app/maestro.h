/*
 * maestro.h
 *
 *  Created on: Oct 28, 2024
 *      Author: Nirgal
 */

#ifndef MAESTRO_H_
#define MAESTRO_H_

#include "stm32g4_adc.h"

//Specific for MAESTRO project
#define ADC_SENSOR_CHANNEL	ADC_2

void MAESTRO_init(void);
void MAESTRO_process_ms(void);
void MAESTRO_process_main(void);

#endif /* MAESTRO_H_ */
