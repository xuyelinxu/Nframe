
/**
* \file     nframe_config.h
* \brief    硬件相关程序，用户需要根据硬件修改nframe_config.c文件内容
*
* \internal
* \par Modification history
* - 1.00 2015-08-16 noodlefighter, first implementation
* \endinternal
*/

/** Includes -----------------------------------------------------------------*/
#include "nframe.h"
#include "nframe_config.h"

#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"

/** 硬件相关子程序 ----------------------------------------------------------*/


/**
* \brief 使用NFTASK模块，需要根据硬件环境编写该部分程序
*/

#ifdef NFTASK_ENABLE

/**
* \brief    NFTASK模块需要使用一个定时器资源,用于定时执行的任务
*
*/
void NFTASK_TimerInit (void)
{
    /**
    * 本程序以stm32f10x硬件为例,应用官方库函数,配置硬件资源TIMER2
    * 使TIM2每隔(NFCONFIG_NFTASK_TIMESLICE)us 产生一次中断
    */

    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TimeBaseStructure;

    /* 开启时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* 预分频系数 1us */
    TimeBaseStructure.TIM_Prescaler = 72;

    /* 向上计数模式 */
    TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    /* 自动重装载寄存器 (NFCONFIG_NFTASK_TIMESLICE)us */
    TimeBaseStructure.TIM_Period = NFTASK_TIMESLICE;

    /* 时钟分频因子 外部来的时钟进行分频 */
    TimeBaseStructure.TIM_ClockDivision = 0x0;

    /* 重复记数次数，存在与高级定时器的使用 */
    TimeBaseStructure.TIM_RepetitionCounter = 0x0;

    /* 配置TIMER */
    TIM_TimeBaseInit(TIM2,&TimeBaseStructure);

    /* 使能定时器的更新事件中断 */
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_ClearFlag(TIM2, TIM_IT_Update);

    /* 打开中断 */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   /* 抢占优先级 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			/* 子优先级 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 启动TIMER */
    TIM_Cmd(TIM2, ENABLE);

}

/**
* \brief TIMER的中断程序 每隔(NFCONFIG_NFTASK_TIMESLICE)us执行一次NFTASK_Isr()
*/
void TIM2_IRQHandler (void)
{
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    NFTASK_Isr();

}


#endif  /* NFCONFIG_NFTASK */

/** Private functions -------------------------------------------------------**/


/* end of file */
