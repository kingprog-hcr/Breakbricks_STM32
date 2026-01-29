/*
 * display.c
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */
#include "stm32g4xx_hal.h"
#include "stdbool.h"
#include "breakbricks.h"
#include "display.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_fonts.h"
#include "stm32g4_systick.h"


//Constantes privées
static const uint16_t ball_colors[BALL_OPTION_NB] = {
		ILI9341_COLOR_RED, 		//BALL_OPTION_NONE
		ILI9341_COLOR_BLUE, 	//BALL_OPTION_GLUE
		ILI9341_COLOR_ORANGE	//BALL_OPTION_JOKER
};


/*
static const uint16_t grid_colors[BRICKSTYLE_NB] = {
		ILI9341_COLOR_WHITE, 	//BRICKSTYLE_NONE
		ILI9341_COLOR_YELLOW, 	//BRICKSTYLE_FULL
		ILI9341_COLOR_GRAY, 	//BRICKSTYLE_ROCK
		ILI9341_COLOR_BLACK, 	//BRICKSTYLE_BOMB
		ILI9341_COLOR_GREEN, 	//BRICKSTYLE_RESIZER
		ILI9341_COLOR_ORANGE, 	//BRICKSTYLE_JOKER
		ILI9341_COLOR_BLUE,		//BRICKSTYLE_GLUE
		ILI9341_COLOR_RED		//BRICKSTYLE_LIFE
};
*/

//Prototype des fonctions privées
static char style2char(uint8_t style);

//_____________________________________

//Fonctions publiques

void DISPLAY_init(void) {
	ILI9341_Init();
	ILI9341_Fill(ILI9341_COLOR_WHITE);
	ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
}


void DISPLAY_refresh_ball(ball_t *ball)
{
	static ball_t displayed_ball;
	static bool displayed_ball_valid = false;

	if (displayed_ball.x != ball->x || displayed_ball.y != ball->y || displayed_ball.size != ball->size || displayed_ball.options != ball->options)
	{
		//Effacement de l'ancienne position de la balle
		if(displayed_ball_valid)
			ILI9341_DrawCircle(displayed_ball.x, ILI9341_WIDTH - displayed_ball.y, displayed_ball.size/2, ILI9341_COLOR_WHITE);
		ILI9341_DrawCircle(ball->x, ILI9341_WIDTH - ball->y, ball->size/2, ball_colors[ball->options]);
		displayed_ball = *ball;
		displayed_ball_valid = TRUE;
	}
}



/*
void DISPLAY_refresh_racket(racket_t *racket)
{

}
*/
/*
void DISPLAY_refresh_grid(grid_t * grid)
{

}
*/

