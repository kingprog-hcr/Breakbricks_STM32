/**
  ******************************************************************************
  * @file    main.c
  * @author  Nirgal
  * @date    03-July-2019
  * @brief   Default main function.
  ******************************************************************************
*/
#include <magic_puzzle_hidden_part.h>
#include "stm32g4xx_hal.h"
#include "stm32g4_uart.h"
#include "stm32g4_gpio.h"
#include "stm32g4_sys.h"
#include "stm32g4_systick.h"
#include "magic_puzzle.h"


int main(void)
{
	//Initialisation de la couche logicielle HAL (Hardware Abstraction Layer)
	//Cette ligne doit rester la premi�re �tape de la fonction main().
	HAL_Init();

	//Initialisation de l'UART2 � la vitesse de 115200 bauds/secondes (92kbits/s) PA2 : Tx  | PA3 : Rx.
		//Attention, les pins PA2 et PA3 ne sont pas reli�es jusqu'au connecteur de la Nucleo.
		//Ces broches sont redirig�es vers la sonde de d�bogage, la liaison UART �tant ensuite encapsul�e sur l'USB vers le PC de d�veloppement.
	BSP_UART_init(UART2_ID,115200);


	//"Indique que les printf sortent vers le p�riph�rique UART2."
	BSP_SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	//Initialisation du port de la led Verte
	BSP_GPIO_pin_config(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH, GPIO_MODE_OUTPUT_PP);


	i_want_to_play_with_magic_puzzle(30871);

	while(1)	//Boucle de t�che de fond
	{

	}
}




