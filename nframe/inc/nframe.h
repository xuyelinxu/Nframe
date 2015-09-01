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


#include "nframe_config.h"


/** Exported MACRO -----------------------------------------------------------*/

/*******************************************************************************
 * \name 通用宏定义
 * @{
 */



typedef unsigned char BOOLEAN;              /**< \brief BOOLEAN */


#ifndef FALSE
#define FALSE            0              /**< \brief false */
#endif

#ifndef TRUE
#define TRUE             (!FALSE)       /**< \brief ture */
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
#define NF_ARRAYEMENTS(array)   (sizeof (array) / sizeof ((array) [0]))

/** \brief 把符号转换为字符串 */
#define NF_STR(s)               #s

/** \brief 把宏展开后的结果转换为字符串 */
#define NF_XSTR(s)              AM_STR(s)

#ifdef NFCONFIG_NFMEM   /* 使用内存管理模块 */

    /** \brief 申请内存 */
    #define NF_MALLOC(size)  ( NFMEM_Malloc(size) )

    /** \brief 为一个vartype类型的变量申请内存 */
    #define NF_MALLOC_VAR(vartype)  ( ((vartype)*)(NFMEM_Malloc(sizeof(vartype))) )

    /** \brief 释放内存 */
    #define NF_FREE(ptr)            ( NFMEM_Free(ptr) )

    /** \brief 内存拷贝 */
    #define NF_MEMCPY(dest,src,size)  ( NFMEM_Memcpy(dest,src,size) )

#else                   /* 使用c标准库自带的malloc函数 */

    /** \brief 申请内存 */
    #define NF_MALLOC(size)  ( malloc(size) )

    /** \brief 为一个vartype类型的变量申请内存 */
    #define NF_MALLOC_VAR(vartype)  ( ((vartype)*)(malloc(sizeof(vartype))) )

    /** \brief 释放内存 */
    #define NF_FREE(ptr)            ( free(ptr) )

    /** \brief 内存拷贝 */
    #define NF_MEMCPY(dest,src,size)  ( memcpy(dest,src,size) )

#endif
/** @} */

/*******************************************************************************
 * \name 内部宏定义类型
 * @{
 */

#if defined ( __CC_ARM   )

    #define NF_INLINE    __inline /*!< ARM Compiler     */

#elif defined ( __ICCARM__ )

    #define NF_INLINE    inline    /*!< IAR Compiler. High optimization mode! */

#elif defined   (  __GNUC__  )

    #define NF_INLINE    inline   /*!< GNU Compiler    */

#elif defined   (  __TASKING__  )

    #define NF_INLINE    inline   /*!< TASKING Compiler */

#endif

/** @} */



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
    NFTASK_Run();
}


/*******************************************************************************
 * extern “C”
 */
#ifdef __cplusplus
}
#endif

#endif

/*****END OF FILE****/
