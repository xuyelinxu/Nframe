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


static char gRxdBuffer[NFDEBUG_BUFFER_SIZE];
static uint16_t gRxdBufferTop = 0;
static uint8_t gRxdComplete = 0;

void NFDEBUG_Init (void)
{

    /* Clear Buffer */
    gRxdBufferTop = 0;

    gRxdComplete = 0;

    NFDEBUG_HardwareInit();
}


/**
* \brief 发送字符串子程序 重定义printf函数
*/
PUTCHAR_PROTOTYPE
{
    NFDEBUG_SendChar(ch);
	return ch;
}

void NFDEBUG_ReciveChar (uint8_t ch)
{
    uint16_t i;

    if(ch == '#')
    {
        /* End of Message */
        gRxdBuffer[gRxdBufferTop] = '\0';
        gRxdComplete = 1;

    }
    else
    {
        /* Save to buffer */
        gRxdBuffer[gRxdBufferTop] = ch;

        gRxdBufferTop++;
        if(gRxdBufferTop >= NFDEBUG_BUFFER_SIZE){   /* Overflow */
            gRxdBufferTop = 0 ;     /* Throw away */
            return;
    }

    }
}

void NFDEBUG_Run (void)
{
    uint8_t i,parmCount,parmsLength,parms[NFDEBUG_COMMAND_PARM_MAX];
    char *pStr;

    if(gRxdComplete){
        gRxdComplete = 0;
        gRxdBufferTop = 0 ; /* Clear Buffer */

        /* Search Command */
        for(i = 0; i<NFDEBUG_COMMANDLIST_SIZE; i++){
            if(strncasecmp( (const char *)(NFDEBUG_CommandList[i].CommandStr),
                            gRxdBuffer,
                            NFDEBUG_CommandList[i].CommandStrLength) == 0){


                pStr = gRxdBuffer + NFDEBUG_CommandList[i].CommandStrLength;
                parmsLength = strlen(pStr);

                parms[0] = pStr;
                if(pStr=='\0'){
                    parmCount = 0;
                }
                else{
                    parmCount = 1;
                }

                for(i=0; i<parmsLength; i++){
                    if(pStr[i]==' '){
                        pStr[i] = '\0';

                        parms[parmCount] = pStr+i+1;
                        parmCount++;

                        /* if overflow, throw away */
                        if(parmCount > NFDEBUG_COMMAND_PARM_MAX){
                            parmCount = NFDEBUG_COMMAND_PARM_MAX;
                            break;
                        }

                    }
                }

                /* call user's command deal func */
                NFDEBUG_CommandList[i].CommandDealFunc(parmCount, parms);

                break;
            }
        }
    }
}
/*****END OF FILE****/
