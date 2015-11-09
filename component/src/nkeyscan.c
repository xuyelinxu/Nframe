/**
* \file     nkeyscan.h
* \brief
*
* \internal
* \par Modification history
* - 1.00 2015-08-16 noodlefighter, first implementation
* \endinternal
*/

/** Includes -----------------------------------------------------------------*/
#include "nkeyscan.h"

#include <string.h>
/** Private typedef ---------------------------------------------------------**/
/** Private define ----------------------------------------------------------**/
/** Private macro -----------------------------------------------------------**/
/** Private variables -------------------------------------------------------**/
/** Private function prototypes ---------------------------------------------**/
/** Private functions -------------------------------------------------------**/

/* 矩阵键盘扫描程序 */
void NKEYSCAN_MODE_MATRIX(NKEYSCAN *nKEYSCAN)
{
    uint8_t row=0,col=0;
    uint8_t num;
    uint32_t colData;
    NKEYSCAN_STORAGE *storageArea = nKEYSCAN->StorageArea;


    for(; row < nKEYSCAN->NumRow ; row++){      /* 行扫描 逐行拉低 */
        nKEYSCAN->WriteRowIo(nKEYSCAN, ~(_BV(row)));      /* 拉低row电平 */
        colData = nKEYSCAN->ReadColIo(nKEYSCAN);        /* 读取col电平 */

        for(; col < nKEYSCAN->NumCol ; col++){
            num = row*(nKEYSCAN->NumCol) + col;

            if((~colData)&_BV(1)){     /* col 此位被拉低 */

                if(storageArea[num] < nKEYSCAN->JitterTime){
                    storageArea[num]++;         /* 自加 直到超过抖动时间 */
                }
                else if(storageArea[num] == 0xFF){    /* 已触发 */
                    /* 无动作 */
                }
                else{                               /* 超过抖动时间 按下动作 */
                    nKEYSCAN->KeyDownFunc(nKEYSCAN, num);
                    storageArea[num] = 0xFF;
                }
            }
            else{                   /* col 此位电平为高 */
                if(storageArea[num] == 0xFF){     /* 弹起动作 */
                    nKEYSCAN->KeyDownFunc(nKEYSCAN, num);
                    storageArea[num] = 0;
                }
            }

            colData = colData>>1;
        }
    }

}

/* 独立按键扫描程序 */
void NKEYSCAN_MODE_SINGLE(NKEYSCAN *nKEYSCAN)
{
    uint8_t i = 0;
    uint32_t ioData = nKEYSCAN->ReadColIo(nKEYSCAN);
    NKEYSCAN_STORAGE *storageArea = nKEYSCAN->StorageArea;

    while(i< nKEYSCAN->KeyNum){
        if(ioData&_BV(1)){      /* 按键电平为高 */
            if(storageArea[i] < nKEYSCAN->JitterTime){
                storageArea[i]++;               /* 自加 直到超过抖动时间 */
            }
            else if(storageArea[i] == 0xFF){    /* 已触发 */
                /* 无动作 */
            }
            else{                               /* 超过抖动时间 按下动作 */
                nKEYSCAN->KeyDownFunc(nKEYSCAN, i);
                storageArea[i] = 0xFF;
            }
        }
        else{                   /* 按键电平为低 */
            if(storageArea[i] == 0xFF){     /* 弹起动作 */
                nKEYSCAN->KeyDownFunc(nKEYSCAN, i);
                storageArea[i] = 0;
            }
        }

        ioData = ioData>>1;
        i++;
    }
}

/*****END OF FILE****/
