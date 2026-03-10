/*
 * display.c
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */
#include "maestro.h"
#include "stm32g4xx_hal.h"
#include "stm32g4_utils.h"
#include "stm32g4_systick.h"
#include "display.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_fonts.h"
#include <stdio.h>


//Prototype des fonctions privées

//_____________________________________

//Fonctions publiques

void DISPLAY_init(void) {
	ILI9341_Init();
	ILI9341_Fill(ILI9341_COLOR_WHITE);
	ILI9341_Rotate(ILI9341_Orientation_Landscape_2);

}
