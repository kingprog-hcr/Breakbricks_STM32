/*
 * snake.h
 *
 *  Created on: Feb 02, 2026
 *      Author: Chrys-Renabell HASSAM & BRIAN DIVINE
 */

#ifndef SNAKE_H
#define SNAKE_H

#include <stdint.h>
#include "stm32g4_utils.h"
#include "stm32g4xx_hal.h"

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240
#define MAX_LENGTH 100
#define SEG_SIZE 15  // taille d'un segment en pixels
#define MAX_BOMBS 10
#define MAX_WALLS 4


void SNAKE_Init(void);
void SNAKE_process_main(void);
void GAME_process(void);


typedef struct {
    int16_t x1; // coin superieur gauche X
    int16_t y1; // coin superieur gauche Y
    int16_t x2; // coin inferieur droit X
    int16_t y2; // coin inferieur droit Y
} segment_t;

typedef struct { // corps du serpent
    segment_t body[MAX_LENGTH];  // tabelau de segments du serpent
    uint16_t length;
    int8_t dir_x; // direction suivant x
    int8_t dir_y; // direction suivant y
} snake_t;


typedef struct {
    int8_t bricks_count;
    segment_t bricks[10];

    int8_t dir_x;
    int8_t dir_y;

} wall_t;




typedef enum{
	INIT,
	WALL_MODE,
	BOMB_MODE,
	VORTEX,
	INFINITE_MODE,
} state_e;

typedef enum {
	JEU,
	PAUSE
} game_mode_e;



#endif
