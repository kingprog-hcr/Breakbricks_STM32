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

// Variables privees

static snake_t snake;
static segment_t apple;
static uint32_t last_move_tick = 0;

// Prototypes prives


static void SNAKE_update_direction(bool up, bool down, bool right, bool left);
static void SNAKE_move(void);


// Initialisation


void SNAKE_Init(void)
{
    int16_t center_x = SCREEN_WIDTH / 2;
    int16_t center_y = SCREEN_HEIGHT / 2;

    snake.length = 3;

    // Direction initiale a droite
    snake.dir_x = 1;
    snake.dir_y = 0;

    for (int i = 0; i < snake.length; i++)
    {
        int16_t seg_x = center_x - i * SEG_SIZE;
        int16_t seg_y = center_y;

        snake.body[i].x1 = seg_x;
        snake.body[i].y1 = seg_y;
        snake.body[i].x2 = seg_x + SEG_SIZE - 1;
        snake.body[i].y2 = seg_y + SEG_SIZE - 1;
    }

    apple.x1 = 50;
    apple.x2 = apple.x1 + SEG_SIZE;
    apple.y1 = 50;
    apple.y2 = apple.y1 + SEG_SIZE;

    BUTTONS_init();
    DISPLAY_init();
}



void SNAKE_process_main(void)
{
    bool button_up    = BUTTON_up_read();
    bool button_down  = BUTTON_down_read();
    bool button_right = BUTTON_right_read();
    bool button_left  = BUTTON_left_read();

    // Mise a jour direction
    SNAKE_update_direction(button_up, button_down, button_right, button_left);

    // Deplacement automatique toutes les 200 ms
    uint32_t current_tick = HAL_GetTick();

    //BSP_systick_get_tick()   fonction qui renvoie le nombre de millisecondes depuis le demarage du programme

    if ((current_tick - last_move_tick) > 200)
    {
        last_move_tick = current_tick;
        SNAKE_move();

        // Affichage

        DISPLAY_refresh_snake(&snake);
        DISPLAY_refresh_apple(&apple);
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

    // DOWN (y augmente)
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

// Deplacement complet

static void SNAKE_move(void)
{
    // Deplacement du corps (de la fin vers le debut)
    for (int i = snake.length - 1; i > 0; i--)
    {
        snake.body[i] = snake.body[i - 1];
    }

    // Deplacement de la tete
    snake.body[0].x1 += snake.dir_x * SEG_SIZE;
    snake.body[0].x2 += snake.dir_x * SEG_SIZE;
    snake.body[0].y1 += snake.dir_y * SEG_SIZE;
    snake.body[0].y2 += snake.dir_y * SEG_SIZE;
}
