
/**
* \file     nframe_config.h
* \brief    前缀： NFCONFIG
*
*           可以根据需要，更改本文件中的宏定义，配置NFrame
*           nframe_config.c中为硬件相关子程序，根据硬件环境，可修改具体实现
*
* \internal
* \par Modification history
* - 1.00 2015-08-16 noodlefighter, first implementation
* \endinternal
*/

/** Define to prevent recursive inclusion ------------------------------------*/
#ifndef _NFRAME_CONFIG_H
#define _NFRAME_CONFIG_H

#ifdef __cplusplus
extern “C” {
#endif

/** Includes -----------------------------------------------------------------*/
#include "nframe.h"



/** Exported MACRO -----------------------------------------------------------*/

/*******************************************************************************
 * \name    模块开关
 * \brief   可以将不需要用到的模块的对应定义项目注释(需注意模块间的耦合关系)
 * @{
 */

#define NFCONFIG_NFMSG              /**< \brief 消息管理模块 */
#define NFCONFIG_NFTASK             /**< \brief 任务管理模块 */
#define NFCONFIG_NFFSM              /**< \brief 有限状态机模块 */
#define NFCONFIG_NFDEBUG            /**< \brief DEBUG模块 */

/** @} */



/*******************************************************************************
 * \name    NFMSG 消息管理模块配置
 * @{
 */

/**< \brief 消息点数量上限 */
#define NFCONFIG_NFMSG_POINT_UPPER      20

/** @} */




/*******************************************************************************
 * \name    NFTASK 任务管理模块配置
 * @{
 */

/**< \brief 任务管理器最小时间片,单位us */
#define NFCONFIG_NFTASK_TIMESLICE       100

/** @} */


/** Exported functions -------------------------------------------------------*/
void NFCONFIG_NFTASK_TimerInit (void);



/*******************************************************************************
 * extern “C”
 */
#ifdef __cplusplus
}
#endif

#endif

/*****END OF FILE****/
