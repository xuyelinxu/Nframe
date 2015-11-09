/**
* \file     nkeyscan.h
* \brief    nkeyscan使用NFRAME的一个通用部件 用于按键检测
*           程序假设在矩阵键盘中
*           MODE_MATRIX中 row在低位 col在高位
*           MODE_SINGLE中 无row存在 NumRow/NumCol/RowWriteIo成员无效
*
* \internal
* \par Modification history
* - 1.00 2015-11-08 noodlefighter, first implementation
* \endinternal
*/

/** Define to prevent recursive inclusion ------------------------------------*/
#ifndef _NKEYSCAN_H_
#define _NKEYSCAN_H_

#ifdef __cplusplus
extern "C" {
#endif

/** Includes -----------------------------------------------------------------*/
#include "nframe.h"

#include "nkeyscan.h"
/** Exported typedef --------------------------------------------------------**/

typedef struct NKEYSCAN NKEYSCAN;

/** \brief 扫描方式 */
typedef void (*NKEYSCAN_MODE)(NKEYSCAN *nKEYSCAN);

/** \brief 读取IO口电平 回调函数 */
typedef uint32_t (*NKEYSCAN_ReadIoFunc)(NKEYSCAN *nKEYSCAN);

/** \brief 设置IO口电平 回调函数 */
typedef void (*NKEYSCAN_WriteIoFunc)(NKEYSCAN *nKEYSCAN, uint32_t ioData);

/** \brief 按键动作 回调函数 */
typedef void (*NKEYSCAN_ActionCallbackFunc)(NKEYSCAN *nKEYSCAN, uint32_t key);

/** \brief 按键扫描暂存数据 */
typedef uint8_t NKEYSCAN_STORAGE;

struct NKEYSCAN{
    NKEYSCAN_MODE                   Mode;
    NKEYSCAN_STORAGE               *StorageArea;
    uint8_t                         KeyNum;
    uint8_t                         NumRow;
    uint8_t                         NumCol;
    uint8_t                         JitterTime;
    NKEYSCAN_ReadIoFunc             ReadColIo;
    NKEYSCAN_WriteIoFunc            WriteRowIo;
    NKEYSCAN_ActionCallbackFunc     KeyDownFunc;
    NKEYSCAN_ActionCallbackFunc     KeyUpFunc;
};

/** Exported MACRO -----------------------------------------------------------*/


/** Exported Functions -------------------------------------------------------*/

NF_INLINE
void NKEYSCAN_Init(NKEYSCAN *nKEYSCAN)
{

    /* 清空暂存数据 */
    memset(nKEYSCAN->StorageArea ,0x00 ,nKEYSCAN->KeyNum);

}

NF_INLINE
void NKEYSCAN_ScanKey(NKEYSCAN *nKEYSCAN, uint32_t ioData)
{
    nKEYSCAN->Mode(nKEYSCAN);
}

/* NKEYSCAN_MODE */
void NKEYSCAN_MODE_MATRIX(NKEYSCAN *nKEYSCAN);
void NKEYSCAN_MODE_SINGLE(NKEYSCAN *nKEYSCAN);


/**
 * extern “C”
 */
#ifdef __cplusplus
}
#endif

#endif

/*****END OF FILE****/
