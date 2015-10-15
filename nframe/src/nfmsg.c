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

/* 模块启用开关 */
#ifdef NFCONFIG_NFMSG

/** Private typedef ---------------------------------------------------------**/

typedef struct{
     NFMSG_pfnHandlerDef   *pfnHandler;     /**< \brief 消息处理函数指针 */
     NFMSG_Type_Enum        MsgType;        /**< \brief 处理的消息类型 */
     NFMSG_HandlerDef      *pNextHandler;   /**< \brief 下一个消息处理 */
} MsgHandlerDef;        /**< \brief 消息处理者 结构体定义 */

typedef struct{
    MsgHandlerDef   *Handler;    /**< \brief 消息处理器 */
} PointDef;                /**< \brief 消息点 结构体定义 */


/** Private define ----------------------------------------------------------**/

/** Private macro -----------------------------------------------------------**/
/** Private variables -------------------------------------------------------**/
static volatile NFMSG_MsgPackDef    msgQueue[NFMSG_QUEUE_LENGTH];
static volatile uint32_t            msgQueueStartIndex = 0;
static volatile uint32_t            msgQueueEndIndex = 0;

/** Private function prototypes ---------------------------------------------**/

/** Private functions -------------------------------------------------------**/

/**
* \brief 创建消息点
*
* \param[in]        pfnMsgHandler 消息处理函数指针  \ref NFMSG_MsgHandlerFunc
*
* \retval 消息点句柄
*/
NFMSG_PointHandle NFMSG_CreatePoint(NFMSG_MsgHandlerFunc pfnMsgHandler)
{
    PointDef *p = NF_MALLOC_VAR(PointDef);    /* 申请内存 */
    if(p == NULL)
        return NULL;

    p->Handler = pfnMsgHandler;

    return (NFMSG_PointHandle)p;
}

/**
* \brief 向消息点添加消息处理函数
*
* \param[in]        PointHandle 消息点句柄
* \param[in]        pfnMsgHandler 消息处理函数指针  \ref NFMSG_MsgHandlerFunc
*
* \retval 消息点句柄
*/
BOOLEAN NFMSG_AddMsgHandler(    NFMSG_PointHandle      PointHandle,
                                NFMSG_MsgHandlerFunc   pfnMsgHandler)
{
    PointDef *pPoint;
    MsgHandlerDef *pHandler;

    pHandler = pPoint->Handler;
    if ( pHandler != NULL)      /* 消息点无Handler */
    {
        pPoint = (PointDef*)PointHandle;    /* 句柄 ---> 消息点指针 */
        pPoint->Handler = NF_MALLOC_VAR(MsgHandlerDef);    /* 申请内存 */
        pHandler = pPoint->Handler;     /* pHandler指向新结构体 */
    }
    else{                       /* 消息点有Handler */

        /* 找到链表尾部 */
        while(pHandler->pNextHandler != NULL)
            pHandler = pHandler->pNextHandler;

        pHandler->pNextHandler = NF_MALLOC_VAR(MsgHandlerDef);   /* 申请内存 */
        pHandler = pHandler->pNextHandler;   /* pHandler指向新结构体 */
    }

    if(pHandler == NULL)
        return false;   /* 申请内存失败 */

    /* 填充消息处理者结构体 */
    pHandler->pNextHandler    = NULL;
    pHandler->pfnHandler     = pfnMsgHandler;

    return true;

}

/**
* \brief 删除一个处理函数
*
* \param[in]        Point   消息点句柄
*
* \retval true      成功
* \retval false     失败
*/
BOOLEAN NFMSG_DeleteMsgHandler(NFMSG_PointHandle      PointHandle,
                            NFMSG_MsgHandlerFunc   pfnMsgHandler)
{
    PointDef *pPoint;
    MsgHandlerDef *pHandler,*pNextHandler,*pNextHandler;

    pPoint = (PointDef*)PointHandle;    /* 句柄 ---> 消息点指针 */
    pHandler = pPoint->Handler;

    if(pHandler->pfnHandler == pfnMsgHandler){   /* 第一个为该函数 */
        NF_FREE(pHandler);
        pPoint->Handler = NULL;
        return true;
    }

    while(pHandler->pNextHandler != NUll){
        if(pHandler->pNextHandler->pfnHandler == pfnMsgHandler){

            /* 找到 释放内存 修改链表*/
            pNextHandler = pHandler->pNextHandler->pNextHandler;
            NF_FREE(pHandler->pNextHandler);
            pHandler->pNextHandler = pNextHandler;

            return true;
        }

        pHandler = pHandler->pNextHandler;
    }

    return false;
}

/**
* \brief 释放链表中所有处理函数
* \param[in]        pHandler
*/
static
void disposeHandlers(MsgHandlerDef *pHandler)
{
    MsgHandlerDef *pNextHandler;

    if(pHandler == NULL)
        return;

    /* 先暂存指向下一个Handler的指针 释放当前Handler 直到最后一个 */
    while( (pNextHandler = pHandler->pNextHandler) != NULL){
        NF_FREE(pHandler);
        pHandler = pNextHandler;
    }

    NF_FREE(pHandler);      /* 释放最后一个Handler */

    return;

}

/**
* \brief 销毁消息点
*
* \param[in]        Point   消息点句柄
*
* \retval true      成功
* \retval false     失败
*/
BOOLEAN NFMSG_DisposePoint(NFMSG_PointHandle PointHandle)
{
    PointDef *pPoint;
    MsgHandlerDef *pHandler;

    pPoint = (PointDef*)PointHandle;    /* 句柄 ---> 消息点指针 */
    pHandler = pPoint->Handler;

    if(pHandler != NUll){
        /* 释放所有Handler内存 */
        disposeHandlers(pHandler);
    }

    /* 释放消息点内存 */
    NF_FREE(pPoint);
}


static NF_INLINE
BOOLEAN msgQueueIsEmpty()
{
    return (msgQueueStartIndex == msgQueueEndIndex);
}

static NF_INLINE
void msgClearQueue()
{
    msgQueueStartIndex = msgQueueEndIndex = 0;
}


BOOLEAN msgQueueAdd(NFMSG_MsgPackDef *msg){
    uint32_t backup = msgQueueEndIndex;
    msgQueue[msgQueueEndIndex] = *msg;

    if(++msgQueueEndIndex >= MSG_QUEUE_LENGTH)   /* Array tail */
        msgQueueEndIndex = 0;

    if(msgQueueEndIndex  == msgQueueStartIndex){ /* Queue full */
        msgQueueEndIndex = backup;
        return false;
    }

    return true;

}


NFMSG_MsgDef *msgQueueDequeue(void){
    uint32_t backup = msgQueueStartIndex;

    msgQueueStartIndex++;
    if(msgQueueStartIndex >= MSG_QUEUE_LENGTH){
        msgQueueStartIndex = 0;
    }

    return &(msgQueue[backup]);
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
        return false;

    if(MsgPack->Target == NULL)
        return false;

    if(DoItNow){
        runMsgHandler(MsgPack);
        return true;
    }
    else{
        /* 对Msg拷贝 */
        pMsg = NF_MALLOC(MsgPack->MsgSize);
        NF_MEMCPY(pMsg, MsgPack->pMsg, MsgPack->MsgSize);
        MsgPack->pMsg = pMsg;

        /* 加入消息队列 */
        return msgQueueAdd(MsgPack);
    }
}


void runMsgHandler(NFMSG_MsgPackDef *pMsgPack)
{
    NFMSG_CallbackMsg callbackMsg;
    MsgHandlerDef *pmsgHandler;

    pmsgHandler = ((PointDef*)(pMsgPack->Target))->Handler;

    while(pmsgHandler != NULL){
        if( (pmsgHandler->MsgType) == (pMsgPack->MsgType)){

            /* 执行Handler */
            callbackMsg = (pmsgHandler->pfnHandler)(pMsgPack);

            /* 执行回调 */
            if(pMsgPack->pfnCallback != NULL)
                (pMsgPack->pfnCallback)(callbackMsg);
        }

        /* 下一个 */
        pmsgHandler = pmsgHandler->pNextHandler;
    }
}

void NFMSG_Run(void)
{
    NFMSG_MsgPackDef *pMsgPack = msgQueueDequeue();       /* 队列中取出消息 */
    if(pMsgPack != NULL){
        runMsgHandler(pMsgPack);        /* 执行 */

        NF_FREE(pMsgPack->pMsg);        /* 销毁其中的Msg */
    }
}

#endif  /* ifdef NFCONFIG_NFMSG */

/**END OF FILE**/
