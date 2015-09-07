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
#include "nfdebug.h"

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

static char buffer_send[NFDEBUG_BUFFER_SIZE];
static char buffer_receive[NFDEBUG_BUFFER_SIZE];
static char *pBuffer_send = buffer_send;
static char *pBuffer_receive = buffer_receive;

void NFDEBUG_Init(void)
{

    NFDEBUG_HardwareInit();


}


/**
* \brief 发送字符串子程序 重定义printf函数
*/
//PUTCHAR_PROTOTYPE
//{
//	USART_SendData(USART1,(u8)ch);
//
//	/* Loop until the end of transmission */
//	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//
//	return ch;
//}






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




/*****END OF FILE****/
