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

#include <string.h>

#ifdef __GNUC__

/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

extern NFDEBUG_CommandDef NFDEBUG_CommandList[NFDEBUG_COMMANDLIST_SIZE];

static volatile char rxdBuffer[NFDEBUG_BUFFER_SIZE];
static volatile uint16_t rxdBufferTop = 0;

#define rxdDealQueueEmpty (rxdDealQueueFront == rxdDealQueueRear)
static volatile char *(rxdDealQueue[NFDEBUG_COMMAND_QUEUE_LENGTH]);
static volatile uint8_t rxdDealQueueFront, rxdDealQueueRear;

static __INLINE
char* rxdDealQueueIn(char *str, uint16_t length)
{
    uint8_t rear;
    char *retStr;

    /* Cal Rear */
    rear = rxdDealQueueRear+1;
    if(rear>=NFDEBUG_COMMAND_QUEUE_LENGTH)
        rear = 0;

    if(rear == rxdDealQueueFront){   /* Full */
        NFDEBUG("rxdDealQueue Full!");
        retStr = NULL;
    }
    else{
        rxdDealQueueRear = rear;
        retStr = (char*)NF_MALLOC(length);
        if(retStr == NULL){
            NFDEBUG("rxdDealQueueIn Error!");
            goto rxdDealQueueIn_EXIT;
        }
        strcpy(retStr, (const char*)rxdBuffer);
        rxdDealQueue[rear] = retStr;
    }

rxdDealQueueIn_EXIT:
    return retStr;
}

static
char* rxdDealQueueOut()
{
    char *retStr;

    if(rxdDealQueueFront == rxdDealQueueRear){  /* Empty */
        retStr = NULL;
    }
    else{
        retStr = (char*)rxdDealQueue[rxdDealQueueFront];

        /* 移动队列头部 */
        if(rxdDealQueueFront == NFDEBUG_COMMAND_QUEUE_LENGTH-1){
            rxdDealQueueFront = 0;
        }
        else{
            rxdDealQueueFront++;
        }
    }

    return retStr;
}

void NFDEBUG_Init (void)
{

    /* Clear Buffer */
    rxdBufferTop = 0;

    rxdDealQueueFront = rxdDealQueueRear = 0;

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
    if(ch == '#')
    {
        /* End of Message */
        rxdBuffer[rxdBufferTop] = '\0';
        rxdDealQueueIn((char*)rxdBuffer, rxdBufferTop);
    }
    else
    {
        /* Save to buffer */
        rxdBuffer[rxdBufferTop] = ch;

        rxdBufferTop++;
        if(rxdBufferTop >= NFDEBUG_BUFFER_SIZE){   /* Overflow */
            rxdBufferTop = 0 ;     /* Throw away */
            return;
        }

    }
}

void NFDEBUG_Run (void)
{
#if NFDEBUG_COMMANDLIST_SIZE != 0
    uint8_t i,j,parmCount,parmsLength;
    char *pStr,*pStrParms,*(parms[NFDEBUG_COMMAND_PARM_MAX]);

    if(!rxdDealQueueEmpty){
        pStr = rxdDealQueueOut();
        rxdBufferTop = 0 ; /* Clear Buffer */

        /* Search Command */
        for(i = 0; i<NFDEBUG_COMMANDLIST_SIZE; i++){
            if(strncasecmp( (const char *)(NFDEBUG_CommandList[i].CommandStr),
                            pStr,
                            NFDEBUG_CommandList[i].CommandStrLength) == 0){


                pStrParms = pStr + NFDEBUG_CommandList[i].CommandStrLength;
                parmsLength = strlen(pStrParms);

                parms[0] = pStrParms;
                parmCount = (pStrParms=='\0') ? 0 : 1 ;

                for(j=0; j<parmsLength; j++){
                    if(pStrParms[j]==' '){
                        pStrParms[j] = '\0';

                        parms[parmCount] = pStrParms+j+1;
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
            }   /* if */
        }   /* for(i = 0; i<NFDEBUG_COMMANDLIST_SIZE; i++) */

        NF_FREE(pStr);
    }
#endif

}

/*****END OF FILE****/
