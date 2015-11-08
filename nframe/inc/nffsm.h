/**
* \file     nffsm.h
* \brief    nffsm为NFrame的一个模块
*
* \internal
* \par Modification history
* - 1.00 2015-08-16 noodlefighter, first implementation
* \endinternal
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _NFFSM_H_
#define _NFFSM_H_


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "nframe.h"

/* Exported Functions --------------------------------------------------------*/

/** \brief FSM用户状态输入消息类型定义 */
#define NFFSM_MSGTYPE_STATE_INTO    0xFE
#define NFFSM_MSGTYPE_STATE_OUT     0xFF

/** \brief FSM状态定义 */
typedef void (*NFFSM_STATE)(uint8_t msgType ,void *msg);

typedef struct
{
    NFFSM_STATE MyState;

} NFFSM;    /** \brief FSM */

/** \brief FSM初始化 */
NF_INLINE
void NFFSM_Init(NFFSM *fsm, NFFSM_STATE initialState)
{
    fsm->MyState = initialState;
    fsm->MyState(NFFSM_MSGTYPE_STATE_INTO, 0);
}

/** \brief FSM状态转换 */
NF_INLINE
void NFFSM_StateTran(NFFSM *fsm, NFFSM_STATE targetState)
{
    fsm->MyState(NFFSM_MSGTYPE_STATE_OUT, 0);
    fsm->MyState = targetState;
    fsm->MyState(NFFSM_MSGTYPE_STATE_INTO, 0);
}

/** \brief FSM用户消息输入 */
NF_INLINE
void NFFSM_MsgIn(NFFSM *fsm, uint8_t msgType ,void *sig)
{
    fsm->MyState(msgType, sig);
}

/**
 * extern “C”
 */
#ifdef __cplusplus
}
#endif

#endif

/*****END OF FILE****/
