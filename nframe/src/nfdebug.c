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

////#pragma import(__use_no_semihosting)
//_sys_exit(int x)
//{
//x = x;
//}
//struct __FILE
//{
//int handle;
///* Whatever you require here. If the only file you are using is */
///* standard output using printf() for debugging, no file handling */
///* is required. */
//};
///* FILE is typedef?d in stdio.h. */
//FILE __stdout;
////FILE __stdin;

#ifdef __GNUC__

/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#ifdef NFDEBUG_COMMAND_ENABLE

extern NFDEBUG_CommandDef NFDEBUG_CommandList;
static char rxdBuffer[NFDEBUG_COMMAND_BUFFER_SIZE];
static uint16_t rxdBufferTop = 0;


#define rxdQueueEmpty (rxdQueueFront == rxdQueueRear)
static volatile char rxdQueue[NFDEBUG_COMMAND_BUFFER_SIZE];
static volatile uint8_t rxdQueueFront, rxdQueueRear;

/**
* \brief rxdQueue 入队操作
*        返回值:
*        0      失败
*        !0     成功
*/
static __INLINE
uint8_t rxdQueueIn(char c)
{
    uint8_t rear,ret;

    /* Cal Rear */
    rear = rxdQueueRear+1;
    if(rear >= NFDEBUG_COMMAND_BUFFER_SIZE)
        rear = 0;

    if(rear == rxdQueueFront){   /* Full */
        ret = 0;
    }
    else{
        rxdQueue[rxdQueueRear] = c;
        rxdQueueRear = rear;
        ret = 1;
    }

    return ret;
}

static __INLINE
char rxdQueueOut()
{
    char ret;

    if(rxdQueueFront == rxdQueueRear){  /* Empty */
        ret = '\0';
    }
    else{
        ret = rxdQueue[rxdQueueFront];  /* Out */

        /* 移动队列头部 */
        rxdQueueFront = (rxdQueueFront == NFDEBUG_COMMAND_BUFFER_SIZE-1)
                        ? 0
                        : rxdQueueFront+1 ;
    }

    return ret;
}


static
void command_deal(char *cmdStr)
{
    uint8_t i,j,parmCount,parmsLength,CommandNum;
    char *pStrParms,*(parms[NFDEBUG_COMMAND_PARM_MAX]);
    NFDEBUG_CommandItemDef *CommandItems;

    CommandNum   = NFDEBUG_CommandList.CommandNum;
    CommandItems = NFDEBUG_CommandList.CommandItems;

    for(i = 0; i<CommandNum; i++){  /* Search Command */
        if(strncasecmp( (const char *)(CommandItems[i].CommandStr),
                        cmdStr,
                        CommandItems[i].CommandStrLength) == 0){


            pStrParms = cmdStr + CommandItems[i].CommandStrLength;
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
            CommandItems[i].CommandDealFunc(parmCount, parms);

            break;
        }   /* if */
    }   /* for(i = 0; i<NFDEBUG_COMMANDLIST_SIZE; i++) */
}

#endif

void NFDEBUG_Run (void)
{

    #ifdef NFDEBUG_COMMAND_ENABLE
    char ch;

    while((ch=rxdQueueOut()) != '\0'){

        //NFDEBUG_("%c,%d,%d", ch, rxdQueueFront, rxdQueueRear);

        if(ch == '#'){
            rxdBuffer[rxdBufferTop] = '\0';     /* End of Message */

            command_deal(rxdBuffer);

            rxdBufferTop = 0 ; /* Clear Buffer */
        }
        else{
            rxdBuffer[rxdBufferTop] = ch;   /* Save to buffer */

            rxdBufferTop++;
            if(rxdBufferTop >= NFDEBUG_COMMAND_BUFFER_SIZE){   /* Overflow */
                rxdBufferTop = 0 ;     /* Throw away */
                return;
            }
        }
    }
    #endif
}

void NFDEBUG_Init (void)
{
    #ifdef NFDEBUG_COMMAND_ENABLE

    /* Clear Buffer */
    rxdBufferTop = 0;

    rxdQueueFront = rxdQueueRear = 0;

    #endif

    NFDEBUG_HardwareInit();
}

void NFDEBUG_ReciveChar (uint8_t ch)
{
    #ifdef NFDEBUG_COMMAND_ENABLE

    if(rxdQueueIn(ch) == 0){
        NFDEBUG("rxdQueue Full!");
    }

    #endif
}


/**
* \brief 发送字符串子程序 重定义printf函数
*/
PUTCHAR_PROTOTYPE
{
    NFDEBUG_SendChar(ch);
	return ch;
}



/*****END OF FILE****/
