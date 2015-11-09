/**
* \file task_manager.h
* \brief 任务管理模块 for stm32f10x
*
* \internal
* \par Modification history
* - 1.10 15-08-16 noodlefighter, 修正TASK_SetupDelete执行时卡死bug,更名nftask
* - 1.01 15-08-11 noodlefighter, 扩展功能,变更模块名为task_manager
* - 1.00 14-11-06 noodlefighter, first implementation
* \endinternal
*/

/** Define to prevent recursive inclusion ------------------------------------*/
#ifndef _NFTASK_H_
#define _NFTASK_H_

/** Includes -----------------------------------------------------------------*/
#include "nframe_common.h"

/** Exported types -----------------------------------------------------------*/

typedef void (*NFTASK_Function)(void);

typedef enum {
    NFTASK_TIME_UNIT_US = 1,                /**< \brief us微秒 */
    NFTASK_TIME_UNIT_MS = 1000,             /**< \brief ms毫秒 */
} NFTASK_Time_Unit_Enum;

typedef enum {
    NFTASK_TYPE_TIMING,        /**< \brief 定时执行 */
    NFTASK_TYPE_TIMINGINT,     /**< \brief 定时中断执行 */
    NFTASK_TYPE_ENDLESS,       /**< \brief 无限循环执行 */
} NFTASK_Type_Enum;

typedef struct{
    NFTASK_Function         Function;           /**< \brief 关联的函数 */
    uint32_t                Time;               /**< \brief 重复间隔 */
    NFTASK_Time_Unit_Enum   NFTASK_Time_Unit;   /**< \brief 重复间隔的单位 */
} NFTASK_TaskTypeDef;

/** Exported constants -------------------------------------------------------*/
/** Exported macro -----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/

/** Exported functions -------------------------------------------------------*/

void NFTASK_Run(void);

uint8_t NFTASK_Add(     NFTASK_TaskTypeDef  *NFTASK_SetupStruct,
                        NFTASK_Type_Enum      NFTASK_TYPE   );

uint8_t NFTASK_Del(NFTASK_Function pfunc, NFTASK_Type_Enum NFTASK_TYPE);

void NFTASK_Isr(void);

#endif  /* _NFTASK_H_ */

/*****END OF FILE****/
