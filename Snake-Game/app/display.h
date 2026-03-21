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

void DISPLAY_refresh_snake(snake_t *snake);
void DISPLAY_refresh_apple(segment_t *apple);
void DISPLAY_string(char *text, uint16_t color_text, uint16_t color_bg, int16_t x, int16_t y);

void DISPLAY_Menu(int8_t *choice);
void DISPLAY_draw_obstacles(segment_t *obs, uint8_t count);


#endif /* DISPLAY_H_ */
