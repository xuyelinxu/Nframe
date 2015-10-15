/**
* \file     nframe.h
* \brief    NFrame是一个单片机裸机编程框架
*           为快速应用开发提供平台
*
*           以C89规范编写，与硬件相关的程序 nframe_config.c 需要用户实现。
*
*           模块        前缀        用途                  依赖
*           nfdebug     NFDEBUG     DEBUG模块             nfconfig
*           nftask      NFTASK      任务管理模块          nfconfig
*           nfmsg       NFMSG       消息管理模块          nfconfig
*           nffsm       NFFSM       有限状态机模块        nfconfig

* \internal
* \par Modification history
* - 1.00 2015-08-16 noodlefighter, first implementation
* \endinternal
*/

/** Define to prevent recursive inclusion ------------------------------------*/
#ifndef NFRAME_H_
#define NFRAME_H_

#ifdef __cplusplus
extern "C" {
#endif

/** Includes -----------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>

//#include "nframe_config.h"
#include "nframe_common.h"

///*******************************************************************************
// * \name 外部宏函数
// * @{
// */
//
//#define NFRAME_Init()
//
//#define NFRAME_DeInit()
//
//#define NFRAME_Run()    NFTASK_Run()
//
///** @} */


//#include "nfdebug.h"
#include "nftask.h"
//#include "nfmsg.h"
//#include "nffsm.h"

/** Exported Functions -------------------------------------------------------*/

static NF_INLINE
void NFRAME_Init(void)
{

}

//static NF_INLINE
//void NFRAME_DeInit(void)
//{
//
//}
//

static NF_INLINE
void NFRAME_Run(void)
{
    #ifdef NFTASK_ENABLE
    NFTASK_Run();
    #endif

    #ifdef NFDEBUG_ENABLE
    NFDEBUG_Run();
    #endif

}

/*******************************************************************************
 * extern “C”
 */
#ifdef __cplusplus
}
#endif

#endif

/*****END OF FILE****/
