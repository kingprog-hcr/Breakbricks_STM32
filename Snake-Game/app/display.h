/*
 * display.h
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_


#include "breakbricks.h"
#include "snake.h"

void DISPLAY_init(void);
void DISPLAY_refresh_ball(ball_t *ball);
void DISPLAY_refresh_racket(racket_t *racket);
void DISPLAY_refresh_grid(grid_t * grid);

void DISPLAY_refresh_snake(snake_t *snake);
void DISPLAY_refresh_apple(segment_t *apple);
void DISPLAY_game_over(void);




#endif /* DISPLAY_H_ */
