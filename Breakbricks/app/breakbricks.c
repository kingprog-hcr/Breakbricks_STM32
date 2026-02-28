/*
 * breakbricks.c
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */
#include "config.h"
#include "breakbricks.h"
#include "button.h"
#include "display.h"
#include "stdbool.h"
#include "stm32g4_systick.h"
#include "stm32g4_utils.h"
#include "display.h"
#include  "stm32g4_systick.h"

//Definitions de macros (constantes)
#define DEFAULT_RACKET_SPEED 6
#define  REFRESH_PERIOD_MS 30

//Variables (privÃes, car elles doivent toutes etre prives)


static ball_t ball; // variable privee de la balle

static racket_t racket;  //variable prive de la racket

static grid_t grid;

static bool flag_refresh = false; // variable qui definit si l'ecran a ete rafraichi ou pas

static int8_t life;

//Protypes de fonctions prives
static void BREAKBRICKS_process_ms(void);
static void BREAKBRICKS_racket_update(bool button_left, bool button_right);
static void BREAKBRICKS_ball_update(bool button_center);
static void BREAKBRICKS_check_collision(void);
static void BREAKBRICKS_breaks(uint8_t b);





//Fonctions publiques

void BREAKBRICKS_init(void)
{

	// on initialise les caracteristiques de la ball
	ball.size = 10;
	ball.x = SCREEN_WIDTH/2;
	ball.y = 40;
	ball.fine_x = ball.x << 3;
	ball.fine_y = ball.y << 3;
	ball.speed_x = 0;
	ball.speed_y = 0;
	ball.joker = 0;
	ball.options = BALL_OPTION_GLUE;

	// same pour la racket

	racket.x = SCREEN_WIDTH/2;
	racket.y = 10 ;
	racket.width = 50;
	racket.height = 10;
	racket.speed = 0;

	life = 3;
	// Remplissage de la grille de brique

	grid.nb_bricks_remaining = 0;

	for (uint8_t y = 0; y < NB_BRICKS_VERTICAL; y++)
	{
	    for (uint8_t x = 0; x < NB_BRICKS_LATERAL; x++)
	    {
	        uint8_t b = y * NB_BRICKS_LATERAL + x;

	        grid.bricks[b].style = BRICKSTYLE_FULL;

	        grid.bricks[b].x1 = (int16_t)(MARGIN_DEFAULT + x * BRICK_WIDTH_DEFAULT);
	        grid.bricks[b].y1 = (int16_t)(SCREEN_HEIGHT + MARGIN_DEFAULT - (y + 1) * BRICK_HEIGHT_DEFAULT);

	        grid.bricks[b].x2 = (int16_t)(-MARGIN_DEFAULT + (x + 1) * BRICK_WIDTH_DEFAULT);
	        grid.bricks[b].y2 = (int16_t)(SCREEN_HEIGHT - MARGIN_DEFAULT - y * BRICK_HEIGHT_DEFAULT);

	        grid.nb_bricks_remaining++;
	    }
	}

		// on definit une brick par style

		grid.bricks[0].style = BRICKSTYLE_BOMB;
		grid.bricks[1].style = BRICKSTYLE_ROCK;
		grid.bricks[2].style = BRICKSTYLE_JOKER;
		grid.bricks[3].style = BRICKSTYLE_RESIZER;
		grid.bricks[4].style = BRICKSTYLE_GLUE;
		grid.bricks[5].style = BRICKSTYLE_LIFE;
		grid.bricks[23].style = BRICKSTYLE_LIFE;
		grid.bricks[9].style = BRICKSTYLE_RESIZER;
		grid.bricks[15].style = BRICKSTYLE_RESIZER;
		grid.bricks[21].style = BRICKSTYLE_RESIZER;

		BUTTONS_init();
		DISPLAY_init();
		BSP_systick_add_callback_function(&BREAKBRICKS_process_ms);

}


void BREAKBRICKS_process_main(void)
{
    bool button_left   = BUTTON_left_read();
    bool button_right  = BUTTON_right_read();
    bool button_center = BUTTON_center_read();

    if(button_left)
        HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_PIN_SET);

    if(button_right)
        HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_PIN_RESET);


    if(flag_refresh){
 // on rafraichi l'ecran uniquement lorsque flag_refresh est sur TRUE.
 // Comme flag_refresh est remise a TRUE
    flag_refresh = false;
    BREAKBRICKS_ball_update(button_center);
    BREAKBRICKS_racket_update(button_left, button_right);

    BREAKBRICKS_check_collision();

    DISPLAY_refresh_ball(&ball);
    DISPLAY_refresh_racket(&racket);
    DISPLAY_refresh_grid(&grid);

    }
    if(life == 0 || grid.nb_bricks_remaining ==0){
    		while(1);
    	}

}



//Fonctions privÃ©es

static void BREAKBRICKS_process_ms(void)
{
	static int8_t t;
	if (t < REFRESH_PERIOD_MS -1){
		t+=1;
	}
	else{
		t = 0 ;
		flag_refresh = TRUE;
	}

}


static void BREAKBRICKS_racket_update(bool button_left, bool button_right)
{
	if (button_left){
		racket.speed = -DEFAULT_RACKET_SPEED;
	}
	else if (button_right){
		racket.speed = +DEFAULT_RACKET_SPEED;
	}
	else{
		racket.speed = 0;
	}
	racket.x += racket.speed;
	if (racket.x < racket.width/2){
		racket.x = racket.width/2;  // on gere les collisions avec le mur gauche
	}
	if (racket.x >  SCREEN_WIDTH - racket.width / 2){ // same mais avec le mur de droite
		racket.x = SCREEN_WIDTH - racket.width/2 ;
	}
}

static void BREAKBRICKS_ball_update(bool button_center)
{
	   ball.fine_x += ball.speed_x;
	   ball.fine_y += ball.speed_y;
	   ball.x = ball.fine_x >> 3;
	   ball.y = ball.fine_y >> 3;

	  // On s assure également que la balle ne peut pas sortir de l écran :
	      if (ball.x < ball.size / 2)
	       ball.x = ball.size / 2;

	      if (ball.x > SCREEN_WIDTH - ball.size / 2)
	       ball.x = SCREEN_WIDTH - ball.size / 2;

	      if (ball.y < ball.size / 2)
	       ball.y = ball.size / 2;

	      if (ball.y > SCREEN_HEIGHT - ball.size / 2)
	       ball.y = SCREEN_HEIGHT - ball.size / 2;

	      if(ball.speed_x == 0 && ball.speed_y==0){

	      		ball.x = racket.x; //balle centrée
	      		ball.y = (int16_t)(racket.y + racket.height / 2 + ball.size / 2 + 1);
	      		ball.fine_x = ball.x << 3;
	      		ball.fine_y = ball.y << 3;
	      		if (button_center)
	      		{
	      		ball.speed_y = -30;
	      		}

}
}


static void BREAKBRICKS_check_collision(void)
{
	//Contact plafond
	if (ball.y + ball.size / 2 >= SCREEN_HEIGHT){
	ball.speed_y = -ball.speed_y;
	}
	//Contacts murs latéraux
	if ((ball.x - ball.size / 2 <= 0) || (ball.x + ball.size / 2 >= SCREEN_WIDTH - 1)){
	ball.speed_x = -ball.speed_x;
	}


	//Contact avec la raquette
	 if ( ball.y - ball.size / 2 < racket.y + racket.height / 2 + 1
	   && ball.x + ball.size / 2 >= racket.x - racket.width / 2 - 1
	   &&  ball.x - ball.size / 2 <= racket.x + racket.width / 2 + 1)
	 {
	//pour ne pas entrer en conflit avec la raquette !
	  ball.y = (int16_t)(racket.y + racket.height / 2 + ball.size / 2 + 1);
	  ball.speed_y = -ball.speed_y;

	  //calcul de la ball.speed_x en fonction du point de contact avec la raquette
	  int16_t percentage = (int16_t)((ball.x - racket.x) * 100 / racket.width);
	  ball.speed_x = percentage;
	 }

	 for(uint8_t b=0; b < NB_MAX_BRICKS; b++){
	 if (grid.bricks[b].style != BRICKSTYLE_NONE)
	 {
	     if  (ball.x >= grid.bricks[b].x1 - ball.size / 2
	      &&  ball.x <= grid.bricks[b].x2 + ball.size / 2
	      &&  ball.y >= grid.bricks[b].y1 - ball.size / 2
	      &&  ball.y <= grid.bricks[b].y2 + ball.size / 2)
	     {
	      BREAKBRICKS_breaks(b);
	      if (ball.x >= grid.bricks[b].x2 || ball.x <= grid.bricks[b].x1)
	       ball.speed_x = -ball.speed_x;
	      if(ball.y >= grid.bricks[b].y2 || ball.y <= grid.bricks[b].y1)
	       ball.speed_y = -ball.speed_y;

	     }

	 }
	   }
	 //Contact avec le sol
	 	if(ball.y - ball.size / 2 <= 0){
	 	ball.speed_x = 0;
	 	ball.speed_y = 0;
	 	life--;
	 	}
}


static void BREAKBRICKS_breaks(uint8_t b){
	if (grid.bricks[b].style != BRICKSTYLE_NONE){

		switch(grid.bricks[b].style){
		case BRICKSTYLE_FULL:

			break;
		case BRICKSTYLE_ROCK:
			// return;
			break;
		case BRICKSTYLE_BOMB:

			break;
		case BRICKSTYLE_RESIZER:
			racket.width = (racket.width == 50 ? 100: 50);

			break;
		case BRICKSTYLE_JOKER:

			break;
		case BRICKSTYLE_GLUE:

			break;
		case BRICKSTYLE_LIFE:
			life ++;

			break;
		case BRICKSTYLE_NB:

			break;

		default:
			break;
		}

		grid.bricks[b].style = BRICKSTYLE_NONE;
		grid.nb_bricks_remaining--;
}
}




