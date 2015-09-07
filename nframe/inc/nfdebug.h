/**
* \file     nfdebug.h
* \brief
*
* \internal
* \par Modification history
* - 1.00 2015-8-17 noodlefighter, first implementation
* \endinternal
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _NFDEBUG_H_
#define _NFDEBUG_H_

/* Includes ------------------------------------------------------------------*/
#include "nframe_common.h"


/* Exported MACRO ------------------------------------------------------------*/

//#define NFDEBUG_Write(parameters)       #ifdef NFDEBUG          \
//                                        printf(parameters);     \
//                                        #endif

/* Exported Functions --------------------------------------------------------*/

void NFDEBUG_Init(void);

void USART_Configuration(void);
void USART_RCC_Configuration(void);
void USART_GPIO_Configuration(void);

#endif

/*****END OF FILE****/
