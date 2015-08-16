
#include "stm32f10x.h"
#include "my_clock.h"
#include "task_manager.h"

volatile uint16_t time;

void clock(void){
    time++;
}

void MYCLOCK_Init(void){
    time = 0;

    TASK_SetupTypeDef TASK_InitStruct;
    TASK_InitStruct.Function = clock;
    TASK_InitStruct.Prescaler = 500;
    TASK_Setup(&TASK_InitStruct, TASK_TYPE_TIMING);
}

void MYCLOCK_StopClock(void){
    TASK_SetupDelete(clock, TASK_TYPE_TIMING);
}

uint16_t MYCLOCK_GetTime(void){
    return time;
}

