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


typedef enum{
	INIT,
	NIVEAU_1,
	NIVEAU_2,
	NIVEAU_3,
	INFINITE_MODE
} state_e;

typedef enum {
	JEU,
	PAUSE
} game_mode_e;



// Variables privees

static snake_t snake;
static segment_t apple;
static uint32_t last_move_tick = 0;
static int16_t score = 0;
static int16_t fps = 200;
static int16_t min_fps = 100;
static bool paused = false;
static bool lastButton = false;

static segment_t obstacles[MAX_OBSTACLES];
static uint8_t obstacle_count = 0;

// Gestion du menu

static int8_t choice = 0;       // choix courant (0 a 3)
static bool last_center = false;
static bool last_up = false;
static bool last_down = false;


// etat global
static state_e state = INIT;
static state_e previous_state = INIT;
static bool first_call = true;






//
// Prototypes prives

static void SNAKE_update_direction(bool up, bool down, bool right, bool left);
static void SNAKE_move(void);
static void Generate_random_apple(void);
static void SNAKE_grow_up(void);
static void SNAKE_check_collisin(void);
static void SNAKE_game_over(void);



static void GAME_menu(void);
static void MENU_update(bool up, bool down, bool center);




// FOnction de la macchine a etat

void GAME_process(void)
{
	// Etat global
	bool entrance = (state != previous_state) || first_call;
	previous_state = state;
	first_call = false;

	switch(state)
	{
	case INIT:
	    if (entrance)
	    {
	        BUTTONS_init();          // Init des boutons
	        DISPLAY_init();          // Init écran
	        last_up = last_down = last_center = false; // Reset front boutons
	        choice = 0;              // choix initial
	    }

	    GAME_menu();  // On appelle le menu chaque tick
	break;
		break;

		case NIVEAU_1:

			if(entrance)
			{
				// 1 seul mur (horizontal decale vers le haut pour eviter le centre)
				obstacle_count = 1;

				obstacles[0].x1 = 4 * SEG_SIZE;   // decale a gauche
				obstacles[0].y1 = 4 * SEG_SIZE;   // decale vers le haut
				obstacles[0].x2 = obstacles[0].x1 + 6 * SEG_SIZE;
				obstacles[0].y2 = obstacles[0].y1 + SEG_SIZE;

				SNAKE_Init();
				DISPLAY_draw_obstacles(obstacles, obstacle_count);
			}

			SNAKE_process_main();
		break;

		case NIVEAU_2:

			if(entrance)
			{
				// 2 murs (horizontal et vertical) decales pour ne pas gener le centre
				obstacle_count = 2;

				// Mur horizontal (vers le haut)
				obstacles[0].x1 = 3 * SEG_SIZE;
				obstacles[0].y1 = 3 * SEG_SIZE;
				obstacles[0].x2 = obstacles[0].x1 + 8 * SEG_SIZE;
				obstacles[0].y2 = obstacles[0].y1 + SEG_SIZE;

				// Mur vertical (vers la droite)
				obstacles[1].x1 = 12 * SEG_SIZE;
				obstacles[1].y1 = 5 * SEG_SIZE;
				obstacles[1].x2 = obstacles[1].x1 + SEG_SIZE;
				obstacles[1].y2 = obstacles[1].y1 + 8 * SEG_SIZE;

				SNAKE_Init();
				DISPLAY_draw_obstacles(obstacles, obstacle_count);
			}

			SNAKE_process_main();
		break;

		case NIVEAU_3:

			if(entrance)
			{
				// 3 murs (forme complexe) decales
				obstacle_count = 3;

				// Mur horizontal haut (deplace vers le haut)
				obstacles[0].x1 = 2 * SEG_SIZE;
				obstacles[0].y1 = 2 * SEG_SIZE;
				obstacles[0].x2 = obstacles[0].x1 + 10 * SEG_SIZE;
				obstacles[0].y2 = obstacles[0].y1 + SEG_SIZE;

				// Mur vertical gauche (deplace vers la gauche)
				obstacles[1].x1 = 1 * SEG_SIZE;
				obstacles[1].y1 = 3 * SEG_SIZE;
				obstacles[1].x2 = obstacles[1].x1 + SEG_SIZE;
				obstacles[1].y2 = obstacles[1].y1 + 8 * SEG_SIZE;

				// Mur horizontal bas (deplace vers le bas)
				obstacles[2].x1 = 2 * SEG_SIZE;
				obstacles[2].y1 = 13 * SEG_SIZE;
				obstacles[2].x2 = obstacles[2].x1 + 10 * SEG_SIZE;
				obstacles[2].y2 = obstacles[2].y1 + SEG_SIZE;

				SNAKE_Init();
				DISPLAY_draw_obstacles(obstacles, obstacle_count);
			}

			SNAKE_process_main();
		break;

		case INFINITE_MODE:

			if(entrance)
			{
				SNAKE_Init();
				obstacle_count = 0;
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
            case 0: state = NIVEAU_1; break;
            case 1: state = NIVEAU_2; break;
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
				state = NIVEAU_1;
			break;

			case 1:
				state = NIVEAU_2;
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


		//ILI9341_Fill(ILI9341_COLOR_WHITE);

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

    //BUTTONS_init();
    DISPLAY_init();

    //while(!BUTTON_center_read());


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
    bool button_center = BUTTON_center_read();

    // Mise à jour direction
    SNAKE_update_direction(button_up, button_down, button_right, button_left);

    // Déplacement automatique toutes les 200 ms
    uint32_t current_tick = HAL_GetTick();

    if(button_center && !lastButton)
          {
        	  paused = !paused;
//        	  int16_t center_x = SCREEN_WIDTH / 2;
//        	  int16_t center_y = SCREEN_HEIGHT / 2;
//
//        	  	DISPLAY_string("Jeu en Pause",
//        	  	               ILI9341_COLOR_RED,
//        	  	               ILI9341_COLOR_WHITE,
//        	  	               center_x - 4 * SEG_SIZE,
//        	  	               center_y);

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


// Generation d'une pomme aleatoire

static void Generate_random_apple(void)
{
    bool valid = false;

    while (!valid)
    {
        valid = true;

        // Position alignee sur la grille
        apple.x1 = (rand() % (SCREEN_WIDTH / SEG_SIZE)) * SEG_SIZE;
        apple.y1 = (rand() % (SCREEN_HEIGHT / SEG_SIZE)) * SEG_SIZE;

        apple.x2 = apple.x1 + SEG_SIZE - 1;
        apple.y2 = apple.y1 + SEG_SIZE - 1;

        // Verifier que la pomme n'est pas sur le serpent
        for (int8_t i = 0; i < snake.length; i++)
        {
            if (snake.body[i].x1 == apple.x1 &&
                snake.body[i].y1 == apple.y1)
            {
                valid = false;
                break;
            }
        }

        // obstacles
        for(uint8_t i = 0; i < obstacle_count; i++)
        {
             if(obstacles[i].x1 == apple.x1 &&
                  obstacles[i].y1 == apple.y1)
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
		 score++;
		 Generate_random_apple();

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

	// Collision obstacles
		for(uint8_t i = 0; i < obstacle_count; i++)
		{
			if(tete.x1 == obstacles[i].x1 &&
			   tete.y1 == obstacles[i].y1)
			{
				SNAKE_game_over();
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


	DISPLAY_string(&text, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE, center_x - (strlen(text)/2) * SEG_SIZE, center_y + 2*SEG_SIZE);

	HAL_Delay(2000);

	// Reset
	score = 0;
	fps = 200;
	snake.length = 3;

	// Retour menu
	state = INIT;
}






