/*
 * snake.c
 *
 *  Created on: Feb 02, 2026
 *      Author: Chrys-Renabell HASSAM
 */

#include "config.h"
#include "snake.h"
#include "button.h"
#include "display.h"
#include "stdbool.h"
#include "stm32g4_systick.h"
#include "stm32g4_utils.h"
#include <stdlib.h>

// Variables privees

static snake_t snake;
static segment_t apple;
static uint32_t last_move_tick = 0;


// Prototypes prives

static void SNAKE_update_direction(bool up, bool down, bool right, bool left);
static void SNAKE_move(void);
static void Generate_random_apple(void);
static void SNAKE_grow_up(void);
static void SNAKE_check_collisin(void);
static void SNAKE_game_over(void);

// Initialisation

void SNAKE_Init(void)
{
	int16_t center_x = (SCREEN_WIDTH / 2 / SEG_SIZE) * SEG_SIZE;
	int16_t center_y = (SCREEN_HEIGHT / 2 / SEG_SIZE) * SEG_SIZE;

    snake.length = 3;

    // Direction initiale : droite
    snake.dir_x = 1;
    snake.dir_y = 0;

    // Initialisation du serpent au centre de l'écran
    for (int8_t i = 0; i < snake.length; i++)
    {
        int16_t seg_x = center_x - i * SEG_SIZE;
        int16_t seg_y = center_y;

        snake.body[i].x1 = seg_x;
        snake.body[i].y1 = seg_y;
        snake.body[i].x2 = seg_x + SEG_SIZE - 1;
        snake.body[i].y2 = seg_y + SEG_SIZE - 1;
    }

    BUTTONS_init();
    DISPLAY_init();

    while(!BUTTON_center_read());
    //while(BUTTON_center_read());

    srand(HAL_GetTick());
    Generate_random_apple();
}


// Boucle principale du jeu

void SNAKE_process_main(void)
{
    bool button_up    = BUTTON_up_read();
    bool button_down  = BUTTON_down_read();
    bool button_right = BUTTON_right_read();
    bool button_left  = BUTTON_left_read();

    // Mise à jour direction
    SNAKE_update_direction(button_up, button_down, button_right, button_left);

    // Déplacement automatique toutes les 200 ms
    uint32_t current_tick = HAL_GetTick();

    if ((current_tick - last_move_tick) > 200)
    {
        last_move_tick = current_tick;

        SNAKE_move();

        // gestion des collision

        SNAKE_check_collisin();
        // Rafraîchissement affichage

        DISPLAY_refresh_snake(&snake);
        DISPLAY_refresh_apple(&apple);

        SNAKE_grow_up();


    }
}


// Gestion direction

static void SNAKE_update_direction(bool up, bool down, bool right, bool left)
{
    // UP
    if (up && snake.dir_y != -1)
    {
        snake.dir_x = 0;
        snake.dir_y = 1;
    }

    // DOWN
    else if (down && snake.dir_y != 1)
    {
        snake.dir_x = 0;
        snake.dir_y = -1;
    }

    // RIGHT
    else if (right && snake.dir_x != -1)
    {
        snake.dir_x = 1;
        snake.dir_y = 0;
    }

    // LEFT
    else if (left && snake.dir_x != 1)
    {
        snake.dir_x = -1;
        snake.dir_y = 0;
    }
}


// Déplacement du serpent

static void SNAKE_move(void)
{
    // Déplacement du corps (de la fin vers le début)
    for (int i = snake.length - 1; i > 0; i--)
    {
        snake.body[i] = snake.body[i - 1];
    }

    // Déplacement de la tête
    snake.body[0].x1 += snake.dir_x * SEG_SIZE;
    snake.body[0].x2 += snake.dir_x * SEG_SIZE;
    snake.body[0].y1 += snake.dir_y * SEG_SIZE;
    snake.body[0].y2 += snake.dir_y * SEG_SIZE;
}


// Génération d'une pomme aléatoire

static void Generate_random_apple(void)
{
    bool valid = false;

    while (!valid)
    {
        valid = true;

        // Position alignée sur la grille
        apple.x1 = (rand() % (SCREEN_WIDTH / SEG_SIZE)) * SEG_SIZE;
        apple.y1 = (rand() % (SCREEN_HEIGHT / SEG_SIZE)) * SEG_SIZE;

        apple.x2 = apple.x1 + SEG_SIZE - 1;
        apple.y2 = apple.y1 + SEG_SIZE - 1;

        // Vérifier que la pomme n'est pas sur le serpent
        for (int8_t i = 0; i < snake.length; i++)
        {
            if (snake.body[i].x1 == apple.x1 &&
                snake.body[i].y1 == apple.y1)
            {
                valid = false;
                break;
            }
        }
    }
}


static void SNAKE_grow_up(void){

	 if (snake.body[0].x1 == apple.x1 && snake.body[0].y1 == apple.y1){
		 snake.length += 1;
		 Generate_random_apple();
	 }
}


static void SNAKE_check_collisin(void){
	segment_t tete = snake.body[0];

	for(int8_t i = 1; i < snake.length; i ++){
		if(tete.x1 == snake.body[i].x1 && tete.y1 == snake.body[i].y1){
			// fin de jeu
			SNAKE_game_over();
		}
	}
		// avec les  mur lateraux
	if(tete.x1 >= (SCREEN_WIDTH / SEG_SIZE ) * SEG_SIZE  || tete.x1 < 0){
		SNAKE_game_over();

	}

	if(tete.y2 > (SCREEN_HEIGHT / SEG_SIZE ) * SEG_SIZE || tete.y1 < 0){
		SNAKE_game_over();
		}
}



static SNAKE_game_over(void){
	DISPLAY_game_over();
	while(1);
}
