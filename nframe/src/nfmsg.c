/**
* \file     nfmsg.h
* \brief    nfmsg为NFrame的一个模块
*
* \internal
* \par Modification history
* - 1.00 2015-08-16 noodlefighter, first implementation
* \endinternal
*/

/** Includes ----------------------------------------------------------------**/
#include "nframe.h"

/** Private typedef ---------------------------------------------------------**/
/** Private define ----------------------------------------------------------**/
/** Private macro -----------------------------------------------------------**/
/** Private variables -------------------------------------------------------**/

#define msgQueueIsEmpty() (msgQueueStartIndex == msgQueueEndIndex)
static volatile NFMSG_MsgPackDef     msgQueue[NFMSG_QUEUE_LENGTH];
static volatile uint32_t             msgQueueStartIndex = 0;
static volatile uint32_t             msgQueueEndIndex = 0;

/* msgBuffer是一个环形缓存区 */
static volatile uint8_t             msgBuffer[NFMSG_MSGBUFFER_LENGTH];
static volatile uint32_t            msgBufferStartIndex = 0;
static volatile uint32_t            msgBufferEndIndex = 0;

/** Private function prototypes ---------------------------------------------**/
void runMsgHandler(NFMSG_MsgPackDef *pMsgPack);
BOOLEAN msgQueueAdd(NFMSG_MsgPackDef *msg);
NFMSG_MsgPackDef *msgQueueDequeue(void);

/** Private functions -------------------------------------------------------**/

/**
* \brief 申请msg缓存
*/
static
void *msgBufferMalloc(uint32_t len)
{
    uint8_t *ptr = NULL;

    NFRAME_InterruptDisable();

    if(msgBufferStartIndex <= msgBufferEndIndex){
        if(NFMSG_MSGBUFFER_LENGTH-msgBufferEndIndex >= len){
            ptr = msgBuffer+msgBufferEndIndex;
            msgBufferEndIndex += len;
        }
        else{
            if(msgBufferStartIndex > len){
                ptr = msgBuffer;
                msgBufferEndIndex = len;
            }
        }
    }
    else{
        if(msgBufferEndIndex+len < msgBufferStartIndex){
            ptr = msgBuffer+msgBufferEndIndex;
            msgBufferEndIndex += len;
        }
    }

    NFRAME_InterruptEnable();
    return (void*)ptr;
}

static
void msgBufferFree(void *ptr, uint32_t len)
{
    msgBufferStartIndex = (uint8_t*)ptr - msgBuffer + len;
}

/**
* \brief 消息队列 入队
*/
static
BOOLEAN msgQueueAdd(NFMSG_MsgPackDef *msg)
{
    uint32_t backup;

    NFRAME_InterruptDisable();

    backup = msgQueueEndIndex;
    msgQueue[msgQueueEndIndex] = *msg;

    if(++msgQueueEndIndex >= NFMSG_QUEUE_LENGTH)   /* Array tail */
        msgQueueEndIndex = 0;

    if(msgQueueEndIndex  == msgQueueStartIndex){ /* Queue full */
        msgQueueEndIndex = backup;

        NFRAME_InterruptEnable();
        return FALSE;
    }

    NFRAME_InterruptEnable();
    return TRUE;

}

/**
* \brief 消息队列 出队
*/
static
NFMSG_MsgPackDef *msgQueueDequeue(void)
{
    uint32_t backup;

    if(msgQueueIsEmpty())
        return NULL;

    NFRAME_InterruptDisable();

    backup = msgQueueStartIndex;

    msgQueueStartIndex++;
    if(msgQueueStartIndex >= NFMSG_QUEUE_LENGTH){
        msgQueueStartIndex = 0;
    }

    NFRAME_InterruptEnable();

    return (NFMSG_MsgPackDef*)&(msgQueue[backup]);
}


/**
* \brief 向消息点发送消息
*
* \param[in]        pfnMsgHandler 消息处理函数  \ref NFMSG_MsgHandlerFunc
*
* \retval
*/
BOOLEAN NFMSG_SendMsg(NFMSG_MsgPackDef *MsgPack, BOOLEAN DoItNow)
{
    void *pMsg;

    if(MsgPack == NULL)
        return FALSE;

    if(MsgPack->pTarget == NULL)
        return FALSE;

    if(DoItNow){
        runMsgHandler(MsgPack);
        return TRUE;
    }
    else{

        /* 拷贝Msg */
        pMsg = msgBufferMalloc(MsgPack->MsgSize);
        if(pMsg != NULL){
            NF_MEMCPY(pMsg, MsgPack->pMsg, MsgPack->MsgSize);
            MsgPack->pMsg = pMsg;

            /* 加入消息队列 */
            return msgQueueAdd(MsgPack);
        }
        else{
            return FALSE;
        }
    }
}

static
void runMsgHandler(NFMSG_MsgPackDef *pMsgPack)
{
    NFMSG_CallbackMsg callbackMsg;
    NFMSG_MsgPointDef *pMsgPoint;
    NFMSG_MsgHandlerDef *pMsgHandler;
    uint8_t handlerNumber;

    pMsgPoint = (NFMSG_MsgPointDef*)(pMsgPack->pTarget);
    handlerNumber = pMsgPoint->HandlerNumber;

    while(handlerNumber--){
        pMsgHandler = &(pMsgPoint->Handler[handlerNumber]);
        if( pMsgHandler->MsgType == pMsgPack->MsgType ){

            /* 执行Handler */
            callbackMsg = (pMsgHandler->pfnHandler)(pMsgPack);

            /* 执行回调 */
            if(pMsgPack->pfnCallback != NULL)
                (pMsgPack->pfnCallback)(callbackMsg);
        }
    }
}

void NFMSG_Run(void)
{
    NFMSG_MsgPackDef *pMsgPack = msgQueueDequeue();       /* 队列中取出消息 */
    if(pMsgPack != NULL){
        runMsgHandler(pMsgPack);        /* 执行对应的消息处理函数 */

        msgBufferFree(pMsgPack->pMsg, pMsgPack->MsgSize);   /* 销毁其中的Msg */
    }
}


void NFMSG_ClearMsgQueue()
{
    msgQueueStartIndex = msgQueueEndIndex = 0;
}

/**END OF FILE**/
