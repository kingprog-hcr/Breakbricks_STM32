/*
 * foo.h
 *
 *  Created on: Mar 8, 2024
 *      Author: Nirgal
 */

#ifndef MAGIC_PUZZLE_H_
#define MAGIC_PUZZLE_H_

#include "stm32g4_systick.h"

void printf_training(void);
void operators_training(uint32_t * data, uint32_t *answers);

void maj2min(char *s);
void full_tab_with_letters(char *tab, uint8_t size);
void play_with_pointer(uint32_t * data);
void play_with_macros(uint32_t * data);

#endif /* MAGIC_PUZZLE_H_ */
