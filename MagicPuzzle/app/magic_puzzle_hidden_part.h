/*
 * guess_picture.h
 *
 *  Created on: Mar 8, 2024
 *      Author: Nirgal
 */

#ifndef MAGIC_PUZZLE_HIDDEN_PART_H_
#define MAGIC_PUZZLE_HIDDEN_PART_H_

#include "magic_puzzle.h"
#include "stm32g4_systick.h"
#include "config.h"

//cette fonction doit �tre appel�e une fois. Elle appelle les tests unitaires de chaque �tape demand�e dans le TP.
//Vous pouvez activer/d�sactiver les tests unitaires des �tapes souhait�es.
void i_want_to_play_with_magic_puzzle(uint16_t student_code);



void check(char *s, char *res);


#endif /* MAGIC_PUZZLE_HIDDEN_PART_H_ */
