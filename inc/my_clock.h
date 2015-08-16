#ifndef _MYCLOCK_H_
#define _MYCLOCK_H_

#include "stm32f10x.h"

void MYCLOCK_Init(void);
uint16_t MYCLOCK_GetTime(void);
void MYCLOCK_StopClock(void);

#endif
