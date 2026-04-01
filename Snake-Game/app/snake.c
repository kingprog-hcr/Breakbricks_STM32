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
#include "TFT_ili9341/stm32g4_ili9341.h"




// Variables privees

static snake_t snake;
static segment_t apple;
static uint32_t last_move_tick = 0;
static int16_t score = 0;
static int16_t fps = 200;
static int16_t min_fps = 100;
static bool paused = false;
static bool lastButton = false;

// Bomb mode
#define MAX_BOMBS 10

static segment_t bombs[MAX_BOMBS];
static bool bomb_active[MAX_BOMBS];
static uint32_t bomb_spawn_tick[MAX_BOMBS];

static uint8_t current_bomb_count = 1;
static uint32_t bomb_lifetime = 5000;   // 5 secondes
static uint32_t bomb_interval = 5000;   // nouvelle bombe toutes les 5 secondes


// Gestion du menu

static int8_t choice = 0;       // choix courant (0 a 3)
static bool last_center = false;
static bool last_up = false;
static bool last_down = false;


// etat global
static state_e state = INIT;
static state_e previous_state =INFINITE_MODE;


//
#define MAX_WALLS 4

static wall_t walls[MAX_WALLS];
static uint8_t wall_count = 0;






//
// Prototypes prives

// Fonctions du fonctionnement basic
static void SNAKE_update_direction(bool up, bool down, bool right, bool left);
static void SNAKE_move(void);
static void Generate_random_apple(segment_t *obj);
static void SNAKE_grow_up(void);
static void SNAKE_check_collisin(void);
static void SNAKE_game_over(void);

// Menu
static void GAME_menu(void);
static void MENU_update(bool up, bool down, bool center);

// Bombe
static void BOMB_spawn(uint8_t i);
static void BOMB_update(void);
static void BOMB_check_collision(void);
static void BOMB_update_difficulty(void);



// FOnction de la machine a etat

void GAME_process(void)
{
	// Etat global
	bool entrance = (state != previous_state);
	previous_state = state;


	switch(state)
	{
	case INIT:
	    if (entrance)
	    {
	        BUTTONS_init();          // Init des boutons
	        DISPLAY_init();          // Init écran
	        last_up = last_down = last_center = false; // Reset front boutons
	        choice = 4;              // choix initial
	    }

	    GAME_menu();  // On appelle le menu chaque tick
	break;
		break;

		case WALL_MODE:

			if(entrance)
			{
				// on a 4 murs, 2 en h , 2 en v

				wall_count = 4;

							wall_t *v_wall_1 = &walls[0];
							wall_t *v_wall_2 = &walls[1];
							wall_t *h_wall_1 = &walls[2];
							wall_t *h_wall_2 = &walls[3];

							v_wall_1->bricks_count = 7;
							v_wall_2->bricks_count = 7;

							v_wall_1->bricks[0].x1 = 3*SEG_SIZE;
							v_wall_1->bricks[0].x2 = 4*SEG_SIZE;
							v_wall_1->bricks[0].y1 = 4*SEG_SIZE;
							v_wall_1->bricks[0].y2 = 5*SEG_SIZE;

							v_wall_2->bricks[0].x1 = 17*SEG_SIZE;
							v_wall_2->bricks[0].x2 = 18*SEG_SIZE;
							v_wall_2->bricks[0].y1 = 4*SEG_SIZE;
							v_wall_2->bricks[0].y2 = 5*SEG_SIZE;

							for(int8_t i = 1 ; i < v_wall_1->bricks_count ; i++)
							{
								v_wall_1->bricks[i].x1 = v_wall_1->bricks[0].x1;
								v_wall_1->bricks[i].x2 = v_wall_1->bricks[0].x2;
								v_wall_1->bricks[i].y1 = v_wall_1->bricks[0].y1 + i*SEG_SIZE;
								v_wall_1->bricks[i].y2 = v_wall_1->bricks[0].y2 + i*SEG_SIZE;

								v_wall_2->bricks[i].x1 = v_wall_2->bricks[0].x1;
								v_wall_2->bricks[i].x2 = v_wall_2->bricks[0].x2;
								v_wall_2->bricks[i].y1 = v_wall_2->bricks[0].y1 + i*SEG_SIZE;
								v_wall_2->bricks[i].y2 = v_wall_2->bricks[0].y2 + i*SEG_SIZE;
							}


							h_wall_1->bricks_count = 10;
							h_wall_2->bricks_count = 10;

							h_wall_1->bricks[0].x1 = 5*SEG_SIZE;
							h_wall_1->bricks[0].x2 = 6*SEG_SIZE;
							h_wall_1->bricks[0].y1 = 2*SEG_SIZE;
							h_wall_1->bricks[0].y2 = 3*SEG_SIZE;

							h_wall_2->bricks[0].x1 = 5*SEG_SIZE;
							h_wall_2->bricks[0].x2 = 6*SEG_SIZE;
							h_wall_2->bricks[0].y1 = 13*SEG_SIZE;
							h_wall_2->bricks[0].y2 = 14*SEG_SIZE;

							for(int8_t i = 1 ; i < h_wall_1->bricks_count ; i++)
							{
								h_wall_1->bricks[i].x1 = h_wall_1->bricks[0].x1 + i*SEG_SIZE;
								h_wall_1->bricks[i].x2 = h_wall_1->bricks[0].x2 + i*SEG_SIZE;
								h_wall_1->bricks[i].y1 = h_wall_1->bricks[0].y1;
								h_wall_1->bricks[i].y2 = h_wall_1->bricks[0].y2;

								h_wall_2->bricks[i].x1 = h_wall_2->bricks[0].x1 + i*SEG_SIZE;
								h_wall_2->bricks[i].x2 = h_wall_2->bricks[0].x2 + i*SEG_SIZE;
								h_wall_2->bricks[i].y1 = h_wall_2->bricks[0].y1;
								h_wall_2->bricks[i].y2 = h_wall_2->bricks[0].y2;
							}



				SNAKE_Init();

				for(int8_t b = 0; b< wall_count; b++){
					DISPLAY_WALL(&walls[b]);
				}

			}

			SNAKE_process_main();
		break;

		case BOMB_MODE:

			if(entrance)
			{
			    SNAKE_Init();

			    uint32_t tick = HAL_GetTick();

			    for(int i = 0; i < MAX_BOMBS; i++)
			    {
			        bomb_active[i] = false;
			        bomb_spawn_tick[i] = tick;
			    }
			}

			SNAKE_process_main();

		break;

		case NIVEAU_3:

			if(entrance)
			{


				SNAKE_Init();

			}

				    SNAKE_process_main();


		break;

		case INFINITE_MODE:

			if(entrance)
			{
				SNAKE_Init();

			}

			SNAKE_process_main();
		break;

		default:
		break;
	}
}



// Fonction menu principale
static void GAME_menu(void)
{
    bool up     = BUTTON_up_read();
    bool down   = BUTTON_down_read();
    bool center = BUTTON_center_read();

    // Détection front montant (appui réel)
    if(up && !last_up)
    {
        choice--;
        if(choice < 0) choice = 3;
    }
    last_up = up;

    if(down && !last_down)
    {
        choice++;
        if(choice > 3) choice = 0;
    }
    last_down = down;

    if(center && !last_center)
    {
        switch(choice)
        {
            case 0: state = WALL_MODE; break;
            case 1: state = BOMB_MODE; break;
            case 2: state = NIVEAU_3; break;
            case 3: state = INFINITE_MODE; break;
        }
    }
    last_center = center;

    DISPLAY_Menu(&choice);
}

static void MENU_update(bool up, bool down, bool center)
{
	// Detection front montant (evite repetition)
	if(up && !last_up)
	{
		choice--;
		if(choice < 0) choice = 3;
	}
	last_up = up;

	if(down && !last_down)
	{
		choice++;
		if(choice > 3) choice = 0;
	}
	last_down = down;

	// Validation avec bouton center
	if(center && !last_center)
	{
		switch(choice)
		{
			case 0:
				state = WALL_MODE;
			break;

			case 1:
				state = BOMB_MODE;
			break;

			case 2:
				state = NIVEAU_3;
			break;

			case 3:
				state = INFINITE_MODE;
			break;

			default:
				break;
		}




	}
	last_center = center;

}



// Initialisation

void SNAKE_Init(void)
{
	int16_t center_x = (SCREEN_WIDTH / 2 / SEG_SIZE) * SEG_SIZE;
	int16_t center_y = (SCREEN_HEIGHT / 2 / SEG_SIZE) * SEG_SIZE;

    snake.length = 3;

    // Direction initiale : droite
    snake.dir_x = 1;
    snake.dir_y = 0;

    // Initialisation du serpent au centre de l'ecran
    for (int8_t i = 0; i < snake.length; i++)
    {
        int16_t seg_x = center_x - i * SEG_SIZE;
        int16_t seg_y = center_y;

        snake.body[i].x1 = seg_x;
        snake.body[i].y1 = seg_y;
        snake.body[i].x2 = seg_x + SEG_SIZE - 1;
        snake.body[i].y2 = seg_y + SEG_SIZE - 1;
    }

    DISPLAY_init();
    srand(HAL_GetTick());
    Generate_random_apple(&apple);
}


// Boucle principale du jeu

void SNAKE_process_main(void)
{
    bool button_up    = BUTTON_up_read();
    bool button_down  = BUTTON_down_read();
    bool button_right = BUTTON_right_read();
    bool button_left  = BUTTON_left_read();
    bool button_center = BUTTON_center_read();

    // Mise à jour direction
    SNAKE_update_direction(button_up, button_down, button_right, button_left);

    // Déplacement automatique toutes les 200 ms
    uint32_t current_tick = HAL_GetTick();

    if(button_center && !lastButton)
          {
        	  paused = !paused;

          }
          lastButton = button_center;
          if(!paused)
          {
        	  if ((current_tick - last_move_tick) > fps)
        	      {
        	          last_move_tick = current_tick;

        	          SNAKE_move();

        	          // gestion des collision

        	          SNAKE_check_collisin();


        	          if (state == BOMB_MODE){
        	                BOMB_update();
        	                BOMB_check_collision();

        	                for(uint8_t i = 0; i < current_bomb_count; i++)
        	                {
        	                    if(bomb_active[i])
        	                  	{
        	                  	  DISPLAY_refresh_bomb(&bombs[i]);
        	                  	 }
        	                }

        	           }


        	          // Rafraîchissement affichage

        	          DISPLAY_refresh_snake(&snake);
        	          DISPLAY_refresh_apple(&apple);
        	          SNAKE_grow_up();


        	      }
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


// Deplacement du serpent

static void SNAKE_move(void)
{
    // Deplacement du corps (de la fin vers le debut)
    for (int8_t i = snake.length - 1; i > 0; i--)
    {
        snake.body[i] = snake.body[i - 1];
    }

    // Deplacement de la tete
    snake.body[0].x1 += snake.dir_x * SEG_SIZE;
    snake.body[0].x2 += snake.dir_x * SEG_SIZE;
    snake.body[0].y1 += snake.dir_y * SEG_SIZE;
    snake.body[0].y2 += snake.dir_y * SEG_SIZE;
}


// Generation d'une pomme aleatoire
static void Generate_random_apple(segment_t *obj)
{
    bool valid = false;

    while (!valid)
    {
        valid = true;

        obj->x1 = (rand() % (SCREEN_WIDTH / SEG_SIZE)) * SEG_SIZE;
        obj->y1 = (rand() % (SCREEN_HEIGHT / SEG_SIZE)) * SEG_SIZE;

        obj->x2 = obj->x1 + SEG_SIZE - 1;
        obj->y2 = obj->y1 + SEG_SIZE - 1;

        // Vérifier serpent
        for (int8_t i = 0; i < snake.length; i++)
        {
            if (snake.body[i].x1 == obj->x1 &&
                snake.body[i].y1 == obj->y1)
            {
                valid = false;
                break;
            }
        }

        // Vérifier murs
        for(uint8_t w = 0; w < wall_count && valid; w++)
        {
            for(uint8_t b = 0; b < walls[w].bricks_count; b++)
            {
                if(obj->x1 == walls[w].bricks[b].x1 &&
                   obj->y1 == walls[w].bricks[b].y1)
                {
                    valid = false;
                    break;
                }
            }
        }

        // Vérifier bombes
        for(uint8_t b = 0; b < MAX_BOMBS && valid; b++)
        {
            if(bomb_active[b])
            {
                if(obj->x1 == bombs[b].x1 &&
                   obj->y1 == bombs[b].y1)
                {
                    valid = false;
                    break;
                }
            }
        }

        // Vérifier la pomme
        if(obj != &apple)
        {
            if(obj->x1 == apple.x1 &&
               obj->y1 == apple.y1)
            {
                valid = false;
            }
        }


    }
}


static void SNAKE_grow_up(void){

	 if (snake.body[0].x1 == apple.x1 && snake.body[0].y1 == apple.y1){
		 snake.length += 1;
		 score++;
		 Generate_random_apple(&apple);

		 if(fps >= min_fps){
			 fps -= 5;
		 }
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


	 // Collision avec les murs
	    for(uint8_t w = 0; w < wall_count; w++)
	    {
	        for(uint8_t b = 0; b < walls[w].bricks_count; b++)
	        {
	            if(tete.x1 == walls[w].bricks[b].x1 &&
	               tete.y1 == walls[w].bricks[b].y1)
	            {
	                SNAKE_game_over();
	            }
	        }
	    }
}





static void SNAKE_game_over(void){

	int16_t center_x = SCREEN_WIDTH / 2;
	int16_t center_y = SCREEN_HEIGHT / 2;

	DISPLAY_string("GAME OVER",
	               ILI9341_COLOR_RED,
	               ILI9341_COLOR_WHITE,
	               center_x - 4 * SEG_SIZE,
	               center_y);

	char score_str[10];
	char text[20];

	itoa(score, score_str, 10);       // convertit 1243 en "1243"
	sprintf(text, "SCORE : %s", score_str);  // concatene "SCORE : " + "1243"


	DISPLAY_string(text, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE, center_x - (strlen(text)/2) * SEG_SIZE, center_y + 2*SEG_SIZE);

	HAL_Delay(2000);

	// Reset
	score = 0;
	fps = 200;
	snake.length = 3;

	// Retour menu
	state = INIT;
	wall_count = 0;
}

static void BOMB_spawn(uint8_t i)
{
    bool valid = false;

    while(!valid)
    {
        valid = true;

        bombs[i].x1 = (rand() % (SCREEN_WIDTH / SEG_SIZE)) * SEG_SIZE;
        bombs[i].y1 = (rand() % (SCREEN_HEIGHT / SEG_SIZE)) * SEG_SIZE;
        bombs[i].x2 = bombs[i].x1 + SEG_SIZE - 1;
        bombs[i].y2 = bombs[i].y1 + SEG_SIZE - 1;

        // Vérifier que la bombe ne tombe pas sur le serpent
        for(int8_t s = 0; s < snake.length; s++)
        {
            if(snake.body[s].x1 == bombs[i].x1 &&
               snake.body[s].y1 == bombs[i].y1)
            {
                valid = false;
                break;
            }
        }
    }

    bomb_active[i] = true;
    bomb_spawn_tick[i] = HAL_GetTick();
}

static void BOMB_update(void)
{
    uint32_t current_tick = HAL_GetTick();

    BOMB_update_difficulty();

    for(uint8_t i = 0; i < current_bomb_count; i++)
    {
        if(!bomb_active[i] && (current_tick - bomb_spawn_tick[i] > bomb_interval))
        {
            BOMB_spawn(i);
        }

        if(bomb_active[i] && (current_tick - bomb_spawn_tick[i] > bomb_lifetime))
        {
            bomb_active[i] = false;
            bomb_spawn_tick[i] = current_tick;
        }
    }
}
static void BOMB_check_collision(void)
{
    for(uint8_t i = 0; i < current_bomb_count; i++)
    {
        if(!bomb_active[i]) continue;

        if(snake.body[0].x1 == bombs[i].x1 &&
           snake.body[0].y1 == bombs[i].y1)
        {
            SNAKE_game_over();
        }
    }
}
static void BOMB_update_difficulty(void)
{
    if(score < 5) current_bomb_count = 1;
    else if(score < 10) current_bomb_count = 3;
    else if(score < 15) current_bomb_count = 4;
    else if(score < 25) current_bomb_count = 6;
    else current_bomb_count = 8;
}


