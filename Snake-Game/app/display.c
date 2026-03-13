/*
 * display.c
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */
#include "stm32g4xx_hal.h"
#include "stdbool.h"
#include "snake.h"
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


//Prototype des fonctions privées
static char style2char(brickstyle_e style);


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





void DISPLAY_refresh_racket(racket_t *racket)
{
	 const uint16_t racket_color = ILI9341_COLOR_BROWN;
	 static uint16_t x1d, x2d, y1d, y2d;  //coordonnées du rectangle de la raquette précédemment affichée
	 static bool initialized = false;  //booléen indiquant si la raquette a déjà été affichée
	 uint16_t x1, x2;    //coordonnées du rectangle de la raquette à afficher

	 if(!initialized)    //si la raquette n'a pas encore été affichée
	 {
	  x1d = racket->x - racket->width/2; //x1d reçoit la valeur du bord gauche de la raquette
	  x2d =   racket->x + racket->width/2;  //x2d reçoit la valeur du bord droit de la raquette
	  y1d = (uint16_t)(ILI9341_WIDTH - racket->y - racket->height/2); //y1d reçoit le bord haut de la raquette
	  y2d = (uint16_t)(ILI9341_WIDTH - racket->y + racket->height/2); //y2d reçoit le bord bas de la raquette
	  ILI9341_DrawFilledRectangle(x1d, y1d, x2d, y2d, racket_color); //On dessine le rectangle de la raquette
	  initialized = TRUE;  //On indique que la raquette a été affichée
	 }
	 else     //si la raquette a déjà été affichée
	 {
	  x1 = racket->x - racket->width/2;   //x1 reçoit la valeur du bord gauche de la raquette à afficher
	  x2 = racket->x + racket->width/2;    //x2 reçoit la valeur du bord droit de la raquette à afficher

	  if(x1<x1d){    //si x1 est à gauche de x1d
		  ILI9341_DrawFilledRectangle(x1, y1d, x1d, y2d, racket_color); //on dessine un rectangle entre x1 et x1d
	  }
	  else if(x1>x1d){    //sinon si x1 est à droite de x1d
		 ILI9341_DrawFilledRectangle(x1d, y1d, x1, y2d, ILI9341_COLOR_WHITE);
	  }
	  if (x2 > x2d){    //si x2 est à droite de x2d
		 ILI9341_DrawFilledRectangle(x2d, y1d, x2, y2d, racket_color);   //on dessine un rectangle entre x2d et x2
	 }
	 else if (x2<x2d){    //sinon si x2 est à gauche de x2d
		 ILI9341_DrawFilledRectangle(x2, y1d, x2d, y2d, ILI9341_COLOR_WHITE);    //on efface un rectangle entre x2 et x2d
	 }
	  x1d = x1;   //x1d reçoit x1 pour le prochain affichage
	  x2d = x2;   //x2d reçoit x2 pour le prochain affichage
	 }

}



void DISPLAY_refresh_snake(snake_t *snake)
{
    static snake_t displayed_snake;
    static bool initialized = false;







    // Dessin nouveau serpent
    if (initialized)
            {
                for (int8_t i = 0; i < displayed_snake.length; i++)
                {
                    int16_t x1 = displayed_snake.body[i].x1;
                    int16_t x2 = displayed_snake.body[i].x2;
                    int16_t y1 = ILI9341_WIDTH - displayed_snake.body[i].y2;
                    int16_t y2 = ILI9341_WIDTH - displayed_snake.body[i].y1;

                    ILI9341_DrawFilledRectangle(x1, y1, x2, y2, ILI9341_COLOR_WHITE);
                }
            }

    // Effacement ancien serpent

    for (int8_t i = 0; i < snake->length; i++)
    {
        int16_t x1 = snake->body[i].x1;
        int16_t x2 = snake->body[i].x2;
        int16_t y1 = ILI9341_WIDTH - snake->body[i].y2;
        int16_t y2 = ILI9341_WIDTH - snake->body[i].y1;

        uint16_t snake_color = (i == 0) ? ILI9341_COLOR_BLUE : ILI9341_COLOR_GREEN;

        ILI9341_DrawFilledRectangle(x1, y1, x2, y2, snake_color);
        ILI9341_DrawRectangle(x1, y1, x2, y2, ILI9341_COLOR_BLACK);
    }



    //Sauvegarde nouvel etat


    displayed_snake = *snake;
    initialized = true;
}
void DISPLAY_refresh_apple(segment_t *apple)
{
    int16_t center_x = (apple->x1 + apple->x2)/2;
    int16_t center_y = ILI9341_WIDTH - ((apple->y1 + apple->y2)/2);
    static segment_t displayed_apple;

    if (displayed_apple.x1 != apple->x1 || displayed_apple.y1 != apple->y1){

        // pomme
        ILI9341_DrawFilledCircle(center_x, center_y, SEG_SIZE/2, ILI9341_COLOR_RED);
        ILI9341_DrawCircle(center_x, center_y, SEG_SIZE/2, ILI9341_COLOR_BLACK);

        // reflet
        ILI9341_DrawFilledCircle(
            center_x - SEG_SIZE/4,
            center_y - SEG_SIZE/4,
            SEG_SIZE/6,
            ILI9341_COLOR_WHITE
        );

        // tige (reste dans le carré de la pomme)
        ILI9341_DrawFilledRectangle(
            center_x - 1,
            center_y - SEG_SIZE/2,
            center_x + 1,
            center_y - SEG_SIZE/2 + 4,
            ILI9341_COLOR_BROWN
        );

        // feuille (collée à la tige mais dans la case)
        ILI9341_DrawFilledCircle(
            center_x + 3,
            center_y - SEG_SIZE/2 + 2,
            2,
            ILI9341_COLOR_GREEN
        );

        displayed_apple = *apple;
    }
}


void DISPLAY_game_over(void){

	int16_t center_x = (SCREEN_WIDTH / 2);
	int16_t center_y = (SCREEN_HEIGHT / 2);
	char text[10] = "GAME OVER";
	int8_t i;
	while(text[i] != '\0'){
	ILI9341_Putc(
				center_x - (4 - i) * SEG_SIZE,
				center_y,
				text[i] ,  // texte
				&Font_11x18,
				ILI9341_COLOR_RED,                 // texte rouge
				ILI9341_COLOR_WHITE // fond
					);
	i ++;
	}
}
