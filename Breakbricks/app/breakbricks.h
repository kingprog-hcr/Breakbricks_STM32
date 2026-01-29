/*
 * breakbricks.h
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */

#ifndef BREAKBRICKS_H_
#define BREAKBRICKS_H_

#include <stdint.h>
#include "stm32g4_utils.h"
#include "stm32g4xx_hal.h"

#define NB_BRICKS_LATERAL	6
#define NB_BRICKS_VERTICAL	4
#define NB_MAX_BRICKS (NB_BRICKS_VERTICAL*NB_BRICKS_LATERAL)

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240
#define MARGIN_DEFAULT 2
#define BRICK_WIDTH_DEFAULT ((SCREEN_WIDTH-(NB_BRICKS_LATERAL+1)*MARGIN_DEFAULT)/NB_BRICKS_LATERAL)
#define BRICK_HEIGHT_DEFAULT (32)


typedef enum {
BALL_OPTION_NONE,
BALL_OPTION_GLUE ,
BALL_OPTION_JOKER,
BALL_OPTION_NB
}ball_options_e;



typedef struct {
	int16_t x;
	int16_t y;
	int16_t fine_x;
	int16_t fine_y;
	int16_t speed_x;
	int16_t speed_y;
	int16_t size;
	ball_options_e options;
	int8_t joker;
} ball_t;






void BREAKBRICKS_init(void);

void BREAKBRICKS_process_main(void);


#endif /* BREAKBRICKS_H_ */
