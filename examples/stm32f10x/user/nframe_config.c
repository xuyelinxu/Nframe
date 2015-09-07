
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
* \brief 使用NFDEBUG模块，需要根据硬件环境编写该部分程序
*/
#ifdef NFDEBUG_ENABLE

/**
* \brief 相关硬件初始化
*/
void NFDEBUG_HardwareInit(void)
{
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE);

    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Init(USART1, &USART_InitStruct);
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);    //使能接收中断
    USART_Cmd(USART1, ENABLE);  //使能串口1

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
* \brief 收发中断处理
* 收到
*/
//void USART1_IRQHandler(void)
//{
//  if(USART_GetFlagStatus(USART1,USART_IT_RXNE) == SET){
//    USART_ClearFlag(USART1,USART_IT_RXNE);
//
//    usart_buff[usart_p] = USART_ReceiveData(USART1);
//    /* 换行符或0 表示一条信息结束 */
//    if(usart_buff[usart_p] == '\r' || usart_buff[usart_p] == '\n' || usart_buff[usart_p] == '#' ){
//      /* throwaway "\r\n" */
//      usart_buff[usart_p] = '\0';
//      usart_p = 0;
//
//      CTRL_MSG msg;
//      msg.MsgType = CTRL_MSG_TYPE_DEBUG_STRING;
//      msg.Msg = (uint32_t)usart_buff;
//      CTRL_msgIn(&msg,0);
//
//    }
//    else{
//      usart_p++;
//    }
//  }
//
//  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
//    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//  }
//
//}

#endif  /* NFDEBUG_ENABLE */

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




/* end of file */
