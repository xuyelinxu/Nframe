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

typedef    intptr_t NFMSG_PointHandle;       /**< \brief 监听者句柄 */
typedef    intptr_t NFMSG_CallbackMsg;

/** \brief 发送消息回调函数 */
typedef void (*NFMSG_CallbackFunc)(NFMSG_CallbackMsg CallbackMsg);

typedef struct{
    NFMSG_PointHandle       Sender;         /**< \brief 发送方 消息点句柄 */
    NFMSG_PointHandle       Target;         /**< \brief 接收方 消息点句柄 */

    NFMSG_Type_Enum         MsgType;        /**< \brief 消息类型 */
    intptr_t                MsgSize;        /**< \brief 消息大小 */
    void                   *pMsg;           /**< \brief 消息 */

    NFMSG_CallbackFunc     *pfnCallback;    /**< \brief 回调函数 */

} NFMSG_MsgPackDef;     /**< \brief 消息包 结构体定义 */


/** \brief  */
typedef NFMSG_CallbackMsg (*NFMSG_MsgHandlerFunc)(NFMSG_MsgPackDef  *pMsgPack);

/** Exported Functions -------------------------------------------------------*/


#define NFMSG_MakeMsgPack(MsgPack, Sender, Target, MsgType, Msg, pfnCallback) \
        {                                                                  \
            MsgPack.Sender  = Sender;                                      \
            MsgPack.Target  = Target;                                      \
            MsgPack.MsgType = MsgType;                                     \
            MsgPack.MsgSize = sizeof(Msg);                                 \
            MsgPack.pMsg    = &Msg;                                        \
        }


NFMSG_PointHandle NFMSG_CreatePoint(NFMSG_MsgHandlerFunc pfnMsgHandler);

BOOLEAN NFMSG_DisposePoint(NFMSG_PointHandle Point);

BOOLEAN NFMSG_SendMsg(NFMSG_MsgPackDef *MsgPack, BOOLEAN DoItNow);

void NFMSG_Run(void);
/**
 * extern “C”
 */
#ifdef __cplusplus
}
#endif

#endif  /* _NFMSG_H_ */

/*****END OF FILE****/
