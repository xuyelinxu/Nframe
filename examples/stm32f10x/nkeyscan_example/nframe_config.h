
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
extern "C" {
#endif

/** Includes -----------------------------------------------------------------*/
#include "nframe_common.h"

#include "stm32f10x_gpio.h"

/** Exported MACRO -----------------------------------------------------------*/

/*******************************************************************************
 * \name    模块开关
 * \brief   可以将不需要用到的模块的对应定义项目注释(需注意模块间的耦合关系)
 * @{
 */

//#define NFMSG_ENABLE              /**< \brief 消息管理模块 */
#define NFTASK_ENABLE             /**< \brief 任务管理模块 */
//#define NFFSM_ENABLE              /**< \brief 有限状态机模块 */
#define NFDEBUG_ENABLE            /**< \brief DEBUG模块 */

/** @} */

/*******************************************************************************
 * \name    NFDEBUG 调试模块配置
 * @{
 */


/**< \brief 开启DEBUG命令功能 */
//#define NFDEBUG_COMMAND_ENABLE

/**< \brief 命令接收缓存区大小 */
#define NFDEBUG_COMMAND_BUFFER_SIZE     200


/**< \brief 最大调试命令参数数量 */
#define NFDEBUG_COMMAND_PARM_MAX        10

/** @} */

/*******************************************************************************
 * \name    NFMSG 消息管理模块配置
 * @{
 */

/**< \brief 消息队列长度 */
#define NFMSG_QUEUE_LENGTH 50

/** @} */




/*******************************************************************************
 * \name    NFTASK 任务管理模块配置
 * @{
 */

/**< \brief 任务管理器最小时间片,单位us */
#define NFTASK_TIMESLICE       100

/** @} */


/*******************************************************************************
 * \name    N 任务管理模块配置
 * @{
 */

/**< \brief 任务管理器最小时间片,单位us */
#define NFTASK_TIMESLICE       100

/** @} */


/** Exported functions -------------------------------------------------------*/


/**
* \brief DEBUG模块会调用这个函数来发送调试文本
*/
__inline
void NFDEBUG_SendChar (uint8_t ch)
{
	USART_SendData(USART1,(uint8_t)ch);

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

}

void NFTASK_TimerInit (void);

void NFDEBUG_HardwareInit(void);

/*******************************************************************************
 * extern “C”
 */
#ifdef __cplusplus
}
#endif

#endif

/*****END OF FILE****/
