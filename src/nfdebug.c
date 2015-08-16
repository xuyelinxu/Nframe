/**
* \file     nfdebug.h
* \brief    nfdebug为NFrame的一个模块
*
* \internal
* \par Modification history
* - 1.00 2015-08-16 noodlefighter, first implementation
* \endinternal
*/

/** Includes -----------------------------------------------------------------*/
#include "debug.h"
#include "string.h"
#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>

#include "ctrl_system.h"

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


/**
  * @brief  开启GPIOA,串口1时钟
  * @param  None
  * @retval None
  * @note  对于某些GPIO上的默认复用功能可以不开启服用时钟，如果用到复用功能的重
           映射，则需要开启复用时钟
  */
void USART_RCC_Configuration(void)
{
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//开复用时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE);
}

/**
  * @brief  init USART GPIO
  * @param  None
  * @retval None
  */
void USART_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;

  GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief  set and enable USART1
  * @param  None
  * @retval None
  */
void USART_Configuration(void)
{
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStructure;

  USART_RCC_Configuration();

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
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//使能接收中断
  USART_Cmd(USART1, ENABLE);//使能串口1

  USART_GPIO_Configuration();
}

PUTCHAR_PROTOTYPE
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1,(u8)ch);

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

	return ch;
}

//recive
u8 usart_buff[50];
u8 usart_p=0;

//void deal(void){
//  u8 i=0;
//  u8 *cmd,*param;
//  while(usart_buff[i]!=' '){
//    if(usart_buff[i]!='\0'){
//      i++;
//    }
//    else{
//      printf("\r\nError CMD!");
//      return;
//    }
//  }
//  usart_buff[i]='\0';
//  cmd=usart_buff;
//  param=usart_buff+i+1;
//
//}

void USART1_IRQHandler(void){
  if(USART_GetFlagStatus(USART1,USART_IT_RXNE) == SET){
    USART_ClearFlag(USART1,USART_IT_RXNE);

    usart_buff[usart_p] = USART_ReceiveData(USART1);
    /* 换行符或0 表示一条信息结束 */
    if(usart_buff[usart_p] == '\r' || usart_buff[usart_p] == '\n' || usart_buff[usart_p] == '#' ){
      /* throwaway "\r\n"*/
      usart_buff[usart_p] = '\0';
      usart_p = 0;

      CTRL_MSG msg;
      msg.MsgType = CTRL_MSG_TYPE_DEBUG_STRING;
      msg.Msg = (uint32_t)usart_buff;
      CTRL_msgIn(&msg,0);

    }
    else{
      usart_p++;
    }
  }

  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  }

}


/*****END OF FILE****/
