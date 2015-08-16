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
#ifndef _NFRAME_H
#define _NFRAME_H

#ifdef __cplusplus
extern “C” {
#endif

/** Includes -----------------------------------------------------------------*/
#include "stdlib.h"

#include "nframe_config.h"

#include "nfdebug.h'
#include "nftask.h'
#include "nfmsg.h'
#include "nffsm.h'

/** Exported MACRO -----------------------------------------------------------*/

/*******************************************************************************
 * \name 通用宏定义
 * @{
 */

#ifndef FALSE
#define FALSE            0              /**< \brief false */
#endif

#ifndef TURE
#define TURE             (!FALSE)       /**< \brief ture */
#endif

#ifndef NULL
#define NULL             ((void *)0)    /**< \brief 空指针           */
#endif

/** @} */

/*******************************************************************************
 * \name 内部宏定义功能 前缀为NF
 * @{
 */

/**
 * \brief 计算数组元素个数
 *
 * \code
 *  int a[] = {0, 1, 2, 3};
 *  int element_a = NF_ARRAYEMENTS(a);    // element_a = 4
 * \endcode
 */
#define NF_ARRAYEMENTS(array)       (sizeof (array) / sizeof ((array) [0]))

/** \brief 把符号转换为字符串 */
#define NF_STR(s)               #s

/** \brief 把宏展开后的结果转换为字符串 */
#define NF_XSTR(s)              AM_STR(s)

/** @} */

/*******************************************************************************
 * \name 内部宏定义类型
 * @{
 */

#if defined ( __CC_ARM   )
    #ifndef __INLINE
    #define __INLINE    __inline /*!< ARM Compiler     */
    #enfif

#elif defined ( __ICCARM__ )
    #ifndef __INLINE
    #define __INLINE    inline    /*!< IAR Compiler. High optimization mode! */
    #endif

#elif defined   (  __GNUC__  )
    #ifndef __INLINE
    #define __INLINE    inline   /*!< GNU Compiler    */
    #endif

#elif defined   (  __TASKING__  )
    #ifndef __INLINE
    #define __INLINE    inline   /*!< TASKING Compiler */
    #endif

#endif

/** @} */


/** Exported Functions -------------------------------------------------------*/

static __INLINE
void NFRAME_Init(void)
{

}
void NFRAME_DeInit(void);

/*******************************************************************************
 * extern “C”
 */
#ifdef __cplusplus
}
#endif

#endif

/*****END OF FILE****/
