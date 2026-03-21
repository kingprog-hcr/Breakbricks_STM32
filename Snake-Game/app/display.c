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


//Prototype des fonctions privées



//_____________________________________

//Fonctions publiques

void DISPLAY_init(void) {
	ILI9341_Init();
	ILI9341_Fill(ILI9341_COLOR_WHITE);
	ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
}


void DISPLAY_refresh_snake(snake_t *snake)
{
    static snake_t displayed_snake;
    static bool initialized = false;






    // Effacement ancien serpent(On efface jsute l'ancien segment du serpent)

    int16_t x1 = displayed_snake.body[displayed_snake.length-1].x1;
    int16_t x2 = displayed_snake.body[displayed_snake.length-1].x2;
    int16_t y1 = ILI9341_WIDTH - displayed_snake.body[displayed_snake.length-1].y2;
    int16_t y2 = ILI9341_WIDTH - displayed_snake.body[displayed_snake.length-1].y1;

    ILI9341_DrawFilledRectangle(x1, y1, x2, y2, ILI9341_COLOR_WHITE);


    // Dessin nouveau serpent

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


void DISPLAY_string(char *text,
                    uint16_t color_text,
                    uint16_t color_bg,
                    int16_t x,
                    int16_t y)

	// Affiche un texte à l'écran avec une couleur et une position données
	// text : chaîne de caractères à afficher
	// color_text : couleur du texte
	// color_bg : couleur du fond
	// x, y : coordonnées sur l'écran

{
    int8_t i = 0;

    while(text[i] != '\0')
    {
        ILI9341_Putc(
            x + i * SEG_SIZE,   // décalage horizontal
            y,
            text[i],            // caractère à afficher
            &Font_11x18,
            color_text,         // couleur du texte
            color_bg            // couleur du fond
        );

        i++;
    }
}



void DISPLAY_Menu(int8_t *choice)
{
    int16_t y = 30; // valeur choisie de maniere arbitraire
    int16_t center_x = SCREEN_WIDTH / 2;
    static int8_t previous_choice = -1;
    // Titre centré
    DISPLAY_string("SNAKE GAME",
                   ILI9341_COLOR_GREEN,
                   ILI9341_COLOR_WHITE,
                   center_x - 75,
                   y);
    					// (strlen("SNAKE GAME")/2) * SEG_SIZE = 75
    char *options[4] = {
        "Niveau 1",
        "Niveau 2",
        "Niveau 3",
        "Infinite"
    };
     if (previous_choice != *choice){


			for(int i = 0; i < 4; i++)
			{
				int16_t box_y1 = y + 40*(i+1);
				int16_t box_y2 = y + 40*(i+2);

				int16_t char_x = center_x - (strlen(options[i])/2) * SEG_SIZE;

				// couler de Fond du cadre
				uint16_t fill_color = (i == *choice) ? ILI9341_COLOR_RED : ILI9341_COLOR_BLUE;

				// Fond du rectangle
				uint16_t border_color = ILI9341_COLOR_WHITE;

				// fond du text
				ILI9341_DrawFilledRectangle(char_x - 15, box_y1, 230, box_y2, fill_color);

				// Bordure
				ILI9341_DrawRectangle(char_x - 15, box_y1, 230, box_y2, ILI9341_COLOR_BLACK);

				// Texte centre dans la box
				DISPLAY_string(options[i],
							   ILI9341_COLOR_BLACK,
							   fill_color,
							   char_x,
							   box_y1 + 8);
			}

			previous_choice = *choice;
   }
}




void DISPLAY_draw_obstacles(segment_t *obs, uint8_t count)
{
    for(uint8_t i = 0; i < count; i++)
    {
        ILI9341_DrawFilledRectangle(
            obs[i].x1,
            ILI9341_HEIGHT - obs[i].y2,
            SEG_SIZE,
            SEG_SIZE,
            ILI9341_COLOR_BLUE
        );
    }
}
