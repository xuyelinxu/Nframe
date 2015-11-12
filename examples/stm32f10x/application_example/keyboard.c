/**
* \file keyboard.c
* \brief
*
*
* \internal
* \par Modification history
* - 1.00 2015-11-11 noodlefighter, first implementation
* \endinternal
*/

/** Includes -----------------------------------------------------------------*/
#include "keyboard.h"
#include "nkeyscan.h"

#include "stm32f10x.h"

#include "mainfsm.h"

/** Private typedef ---------------------------------------------------------**/
/** Private define ----------------------------------------------------------**/
/** Private macro -----------------------------------------------------------**/
#define MATRIXKB_PORT GPIOA
#define MATRIXKB_PORT_PIN_ROW   GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3
#define MATRIXKB_PORT_PIN_COL   GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7

/** Private variables -------------------------------------------------------**/

/* 这里用矩阵键盘做一个小键盘 */
const char keymap_matrix[] = {
    KEYBOARD_KeyVal_7,
    KEYBOARD_KeyVal_8,
    KEYBOARD_KeyVal_9,
    KEYBOARD_KeyVal_plus,

    KEYBOARD_KeyVal_4,
    KEYBOARD_KeyVal_5,
    KEYBOARD_KeyVal_6,
    KEYBOARD_KeyVal_minus,

    KEYBOARD_KeyVal_1,
    KEYBOARD_KeyVal_2,
    KEYBOARD_KeyVal_3,
    KEYBOARD_KeyVal_multi,

    KEYBOARD_KeyVal_esc,
    KEYBOARD_KeyVal_0,
    KEYBOARD_KeyVal_equal,
    KEYBOARD_KeyVal_div
};

static NKEYSCAN matrixKB;       /* 矩阵键盘 */

static NKEYSCAN_STORAGE matrixKB_storeArea[16];

/** Private function prototypes ---------------------------------------------**/
uint32_t readColIo(NKEYSCAN *nKEYSCAN);
void writeRowIo(NKEYSCAN *nKEYSCAN, uint32_t ioData);
void keyDownAction(NKEYSCAN *nKEYSCAN, uint32_t key);
void keyUpAction(NKEYSCAN *nKEYSCAN, uint32_t key);
void scankey(void);

/** Private functions -------------------------------------------------------**/

void KEYBOARD_Init(void)
{
    NFTASK_TaskTypeDef taskDef;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* GPIO Init */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = MATRIXKB_PORT_PIN_ROW;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MATRIXKB_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = MATRIXKB_PORT_PIN_COL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(MATRIXKB_PORT, &GPIO_InitStructure);

    /* NKEYSCAN Init */
    matrixKB.Mode           =   NKEYSCAN_MODE_MATRIX;
    matrixKB.KeyNum         =   16;                     /* 16颗按键 */
    matrixKB.NumCol         =   4;                      /* 4列 */
    matrixKB.NumRow         =   4;                      /* 4行 */
    matrixKB.JitterTime     =   4;                      /* 4*5ms */
    matrixKB.StorageArea    =   matrixKB_storeArea;
    matrixKB.ReadColIo      =   readColIo;                 /* 读IO(列)回调函数 */
    matrixKB.WriteRowIo     =   writeRowIo;                /* 写IO(行)回调函数 */
    matrixKB.KeyDownFunc    =   keyDownAction;          /* 按下触发 */
    matrixKB.KeyUpFunc      =   NULL;//keyUpAction;            /* 弹起触发 */
    NKEYSCAN_Init(&matrixKB);

    /* Crate a task of NFTASK */
    taskDef.Function          = scankey;
    taskDef.NFTASK_Time_Unit  = NFTASK_TIME_UNIT_MS;    /* 单位ms */
    taskDef.Time              = 5;                      /* 5ms扫描一次 */
    NFTASK_Add(&taskDef, NFTASK_TYPE_TIMING);

}

void KEYBOARD_DeInit(void)
{
    NFTASK_Del(scankey, NFTASK_TYPE_TIMING);
}

/* 调用扫描程序 */
void scankey(void)
{
    NKEYSCAN_ScanKey(&matrixKB);
}


uint32_t readColIo(NKEYSCAN *nKEYSCAN)
{
    uint32_t ioData;

    ioData = GPIO_ReadInputData(MATRIXKB_PORT);
    ioData = ioData>>4;

    return ioData;
}

void writeRowIo(NKEYSCAN *nKEYSCAN, uint32_t ioData)
{

    /* 为matrixKB服务 取低四位写 */
    GPIO_ResetBits(MATRIXKB_PORT, MATRIXKB_PORT_PIN_ROW);
    GPIO_SetBits(MATRIXKB_PORT, (uint16_t)(ioData&0x000F));

}

void keyDownAction(NKEYSCAN *nKEYSCAN, uint32_t key)
{
    char keyValue = keymap_matrix[key];

    NFMSG_MsgPackDef pack;

    pack.pTarget = &MainFsm_MsgPoint;
    pack.pfnCallback = NULL;
    pack.pMsg = &keyValue;
    pack.MsgSize = sizeof(char);
    pack.MsgType = MainFsm_MsgType_Key;

    NFMSG_SendMsg(&pack, FALSE);

    //NFDEBUG_("KeyDown: %d" ,keyValue);
}
//
//void keyUpAction(NKEYSCAN *nKEYSCAN, uint32_t key)
//{
//    char keyValue;
//
//    keyValue = keymap_matrix[key];
//
//    NFDEBUG_("KeyUp: %c" ,keyValue);
//}

/* end of file */
