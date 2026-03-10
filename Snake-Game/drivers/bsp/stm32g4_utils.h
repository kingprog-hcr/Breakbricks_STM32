/**
 *******************************************************************************
 * @file 	stm32g4_utils.h
 * @author 	jjo
 * @date 	Mar 14, 2024
 * @brief 	Collection of tools used in BSP drivers
 * 			Adapted from macro_types.h file (S.Poiraud)
 *******************************************************************************
 */

/* Includes-------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/

#ifndef BSP_STM32G4_UTILS_H_
#define BSP_STM32G4_UTILS_H_

#define TRUE 					true		//soyons tolérant... ;)
#define FALSE 					false

#define HIGHINT(x)				((uint8_t)(((x) >> 8) & 0xFF))
#define LOWINT(x)				((uint8_t)((x) & 0xFF))
#define U16FROMU8(high,low)		((((uint16_t)(high))<<8)|(uint16_t)(low))
#define U32FROMU16(high,low)		((((uint32_t)(high))<<16)|(uint32_t)(low))
#define U32FROMU8(higher,high,low,lower)		((((uint32_t)(higher))<<24)|(((uint32_t)(high))<<16)|(((uint32_t)(low))<<8)|(uint32_t)(lower))
#define BITS_ON(var, mask)		((var) |= (mask))
/* ~0 est le complement à 1 de 0, donc pour 16 bits OxFFFF) */
/* ~0 ^ mask permet d'etre indépendant de la taille (en bits) de ~mask */
#define BITS_OFF(var, mask)		((var) &= ~0 ^ (mask))
#define BIT_SET(var, bitno)		((var) |= (1 << (bitno)))
#define BIT_CLR(var, bitno)		((var) &= ~(1 << (bitno)))
#define BIT_TEST(data, bitno)	(((data) >> (bitno)) & 0x01)
#define MIN(a, b)				(((a) > (b)) ? (b) : (a))
#define MAX(a, b)				(((a) > (b)) ? (a) : (b))

#define nop()					__asm__("nop")
/* la fonction valeur absolue pour des entiers */
#define ABSOLUTE(x)					(((x) >= 0) ? (x) : (-(x)))
#define MODULO(x, N) ((x % N + N) % N)		//vrai modulo, avec un résultat non signé. Car l'opérateur % renvoit le reste signé de la division entière.

// Debug defines
#if !TRACE
	#undef assert
	#define assert(condition) if(!(condition)) {printf("assert failed file " __FILE__ " line %d : %s", __LINE__ , #condition ); NVIC_SystemReset();}
#else
	#include "trace/Trace.h"
	#define assert(condition) if(!(condition)) {trace_printf("assert failed file " __FILE__ " line %d : %s", __LINE__ , #condition ); NVIC_SystemReset();}
#endif

#define debug_printf(...)			printf(__VA_ARGS__)

//Macro permettant d'activer un envoi sur l'UART de ce qui est affich� sur le LCD...
//Utile si on a pas de LCD par exemple.
#define PRINT_ON_UART_LCD_DISPLAY

#ifdef PRINT_ON_UART_LCD_DISPLAY
	#define lcd_to_uart_printf(...)		printf(__VA_ARGS__)
#else
	#define lcd_to_uart_printf(...)		(void)(0)
#endif

/* Public types --------------------------------------------------------------*/

//Enum renvoyée par une machine à états.
typedef enum
{
	IN_PROGRESS = 0,
	END_OK,
	END_ERROR,
	END_TIMEOUT
}running_t;

typedef void(*callback_fun_t)(void);	//Type pointeur sur fonction

#endif /* BSP_STM32G4_UTILS_H_ */
