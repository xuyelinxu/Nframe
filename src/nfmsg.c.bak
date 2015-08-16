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

typedef struct {
     NFMSG_pfnHandlerDef   *pfnHandler;     /**< \brief 消息处理函数指针 */
     NFMSG_HandlerDef      *NextHandler;    /**< \brief 下一个消息处理 */
} NFMSG_HandlerDef;        /**< \brief 消息处理 结构体定义 */

typedef struct {
    NFMSG_HandlerDef   *Handler;    /**< \brief 消息处理器 */
} NFMSG_PointDef;                /**< \brief 消息监听者 结构体定义 */

/** Private define ----------------------------------------------------------**/

/** Private macro -----------------------------------------------------------**/
/** Private variables -------------------------------------------------------**/


/** Extern variables --------------------------------------------------------**/
/** Private function prototypes ---------------------------------------------**/

/** Private functions -------------------------------------------------------**/

/**
* \brief 创建消息点
*
* \param[in]        pfnMsgHandler 消息处理函数  \ref NFMSG_MsgHandlerFunc** \retval 消息点句柄
*/
NFMSG_PointHandle NFMSG_CreatePoint(NFMSG_MsgHandlerFunc pfnMsgHandler)
{

}

/**
* \brief 销毁消息点
*
* \param[in]        Point   消息点句柄** \retval true      成功
* \retval false     失败
*/
bool NFMSG_DisposePoint(NFMSG_PointHandle Point)
{

}

/**
* \brief 向消息点发送消息
*
* \param[in]        pfnMsgHandler 消息处理函数  \ref NFMSG_MsgHandlerFunc** \retval
*/
NFMSG_MsgID NFMSG_SendMsg(  NFMSG_PointHandle       Target,
                            NFMSG_PointHandle       Sender,
                            void                   *pMsg,
                            uint32_t                MsgSize,
                            NFMSG_CallbackFunc     *pfnCallback,
                            NFMSG_Priority_Enum     Priority    )
{

}


#endif  /* NFCONFIG_NFMSG */

/**END OF FILE**/
