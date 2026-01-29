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

//Definitions de macros (constantes)



//Variables (privÃes, car elles doivent toutes etre prives)




//Protypes de fonctions prives
static void BREAKBRICKS_process_ms(void);
static void BREAKBRICKS_racket_update(bool button_left, bool button_right);
static void BREAKBRICKS_ball_update(bool button_center);
static void BREAKBRICKS_check_collision(void);

static ball_t ball;




//Fonctions publiques

void BREAKBRICKS_init(void)
{
	BUTTONS_init();

	ball.size = 10;
	ball.x = SCREEN_WIDTH/2;
	ball.y = 40;
	ball.fine_x = ball.x << 3;
	ball.fine_y = ball.y << 3;
	ball.speed_x = 0;
	ball.speed_y = -30;
	ball.joker = 0;
	ball.options = BALL_OPTION_GLUE;

	DISPLAY_init();

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

    BREAKBRICKS_ball_update(button_center);
    BREAKBRICKS_check_collision();
    DISPLAY_refresh_ball(&ball);
}



//Fonctions privÃ©es

static void BREAKBRICKS_process_ms(void)
{

}


static void BREAKBRICKS_racket_update(bool button_left, bool button_right)
{

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
}


static void BREAKBRICKS_check_collision(void)
{
	//Contact plafond
	if (ball.y + ball.size / 2 >= SCREEN_HEIGHT)
	ball.speed_y = -ball.speed_y;
	//Contacts murs latéraux
	if ((ball.x - ball.size / 2 <= 0) || (ball.x + ball.size / 2 >= SCREEN_WIDTH - 1))
	ball.speed_x = -ball.speed_x;
	//Contact avec le sol
	if(ball.y - ball.size / 2 <= 0)
	ball.speed_y = -ball.speed_y;
}

