/**
* \file     nfdebug.h
* \brief
*
* \internal
* \par Modification history
* - 1.00 2015-11-08 noodlefighter, first implementation
* \endinternal
*/

/** Define to prevent recursive inclusion ------------------------------------*/
#ifndef _NKEYSCAN_H_
#define _NKEYSCAN_H_

/** Includes -----------------------------------------------------------------*/
#include "nframe.h"

/** Exported typedef --------------------------------------------------------**/

/** \brief 扫描方式 */
typedef void (*NKEYSCAN_TYPE)(NKEYSCAN *nKEYSCAN, uint32_t ioData);

/** \brief 按键扫描暂存数据 */
typedef uint8_t NKEYSCAN_STORAGE;

typedef struct{
    NKEYSCAN_TYPE           Type;
    NKEYSCAN_STORAGE       *StorageArea;
    uint8_t                 KeyNum;
    uint8_t                 NumRow;
    uint8_t                 NumCol;
    uint8_t                 JitterTime;
    NFMSG_MsgPointDef      *pMsgPoint;

} NKEYSCAN;

/** Exported MACRO -----------------------------------------------------------*/


/** Exported Functions -------------------------------------------------------*/

void NKEYSCAN_Init(NKEYSCAN *nKEYSCAN);

NF_INLINE
void NKEYSCAN_IoData(NKEYSCAN *nKEYSCAN, uint32_t ioData)
{
    nKEYSCAN->Type(nKEYSCAN, ioData);
}

/* NKEYSCAN_TYPE */
void NKEYSCAN_TYPE_MATRIX(NKEYSCAN *nKEYSCAN, uint32_t ioData);
void NKEYSCAN_TYPE_SINGLE(NKEYSCAN *nKEYSCAN, uint32_t ioData);

#endif

/*****END OF FILE****/
