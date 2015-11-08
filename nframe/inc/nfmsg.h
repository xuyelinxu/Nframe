/**
* \file     nfmsg.h
* \brief    nfmsg为NFrame的一个模块
*
* \internal
* \par Modification history
* - 1.00 2015-08-16 noodlefighter, first implementation
* \endinternal
*/

/** Define to prevent recursive inclusion ------------------------------------*/
#ifndef _NFMSG_H_
#define _NFMSG_H_

#ifdef __cplusplus
extern "C" {
#endif

/** Includes -----------------------------------------------------------------*/
#include "nframe.h"

/** Exported typedef --------------------------------------------------------**/

typedef    intptr_t NFMSG_CallbackMsg;

/** \brief 发送消息回调函数 */
typedef void (*NFMSG_CallbackFunc)(NFMSG_CallbackMsg CallbackMsg);

typedef struct NFMSG_MsgPackDef NFMSG_MsgPackDef;

/** \brief  */
typedef NFMSG_CallbackMsg (*NFMSG_MsgHandlerFunc)(NFMSG_MsgPackDef  *pMsgPack);

typedef struct {
     NFMSG_MsgHandlerFunc   pfnHandler;     /**< \brief 消息处理函数指针 */
     uint8_t                MsgType;        /**< \brief 处理的消息类型 */
} NFMSG_MsgHandlerDef;        /**< \brief 消息处理者 结构体定义 */

typedef struct{
    uint8_t                 HandlerNumber;  /**< \brief 消息处理器数量 */
    NFMSG_MsgHandlerDef    *Handler;        /**< \brief 消息处理器数组 */
} NFMSG_MsgPointDef;        /**< \brief 消息点 结构体定义 */

struct NFMSG_MsgPackDef{
    NFMSG_MsgPointDef    *pSender;         /**< \brief 发送方 消息点句柄 */
    NFMSG_MsgPointDef    *pTarget;         /**< \brief 接收方 消息点句柄 */

    uint8_t                 MsgType;        /**< \brief 消息类型 */
    intptr_t                MsgSize;        /**< \brief 消息大小 */
    void                   *pMsg;           /**< \brief 消息 */

    NFMSG_CallbackFunc      pfnCallback;    /**< \brief 回调函数 */

};     /**< \brief 消息包 结构体定义 */









/** Exported Functions -------------------------------------------------------*/


#define NFMSG_MakeMsgPack(MsgPack, Sender, Target, MsgType, Msg, pfnCallback) \
        {                                                                  \
            MsgPack.Sender  = Sender;                                      \
            MsgPack.Target  = Target;                                      \
            MsgPack.MsgType = MsgType;                                     \
            MsgPack.MsgSize = sizeof(Msg);                                 \
            MsgPack.pMsg    = &Msg;                                        \
        }


BOOLEAN NFMSG_SendMsg(NFMSG_MsgPackDef *MsgPack, BOOLEAN DoItNow);

void NFMSG_Run(void);

void NFMSG_ClearMsgQueue(void);

/**
 * extern “C”
 */
#ifdef __cplusplus
}
#endif

#endif  /* _NFMSG_H_ */

/*****END OF FILE****/
