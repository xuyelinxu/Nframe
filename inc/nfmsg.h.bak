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
extern “C” {
#endif

/** Includes -----------------------------------------------------------------*/
#include "nframe.h"

/** Exported typedef --------------------------------------------------------**/

typedef NFMSG_PointHandle   uint32_t;       /**< \brief 监听者句柄 */
typedef NFMSG_MsgID         uint32_t;       /**< \brief 消息ID */

/** \brief  */
typedef void (*NFMSG_MsgHandlerFunc)(NFMSG_PointHandle *Sender,
                                     NFMSG_MsgID        MsgID,
                                     void              *Msg));

/** \brief 发送消息回调函数 */
typedef void (*NFMSG_CallbackFunc)(NFMSG_MsgID);

typedef enum {
    NFMSG_Priority_1,
    NFMSG_Priority_2,
    NFMSG_Priority_3,
    NFMSG_Priority_4,
    NFMSG_Priority_5,
    NFMSG_Priority_6,
    NFMSG_Priority_7,
    NFMSG_Priority_8,
} NFMSG_Priority_Enum;

/** Exported Functions -------------------------------------------------------*/

NFMSG_PointHandle NFMSG_CreatePoint(NFMSG_MsgHandlerFunc pfnMsgHandler);

bool NFMSG_DisposePoint(NFMSG_PointHandle Point);

NFMSG_MsgID NFMSG_SendMsg(  NFMSG_PointHandle       Target,
                            NFMSG_PointHandle       Sender,
                            void                   *pMsg,
                            uint32_t                MsgSize,
                            NFMSG_CallbackFunc     *pfnCallback,
                            NFMSG_Priority_Enum     Priority    );

/**
 * extern “C”
 */
#ifdef __cplusplus
}
#endif

#endif  /* _NFMSG_H_ */

/*****END OF FILE****/
