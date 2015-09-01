/**
* \file     nftsak.c
* \brief    nftask为NFrame的一个模块
*
*   前缀： NFTASK_
*   主要功能：
*   1. 无限循环执行子程序（相当于主循环）
*   2. 定时执行子程序（中断执行）
*   3. 定时执行子程序（轮询执行）
*
* \internal
* \par Modification history
* - 1.10 15-08-16 noodlefighter, 修正TASK_SetupDelete执行时卡死bug,更名nftask
* - 1.01 15-08-11 noodlefighter, 扩展功能,变更模块名为task_manager
* - 1.00 14-11-06 noodlefighter, first implementation
* \endinternal
*/

/** Includes ----------------------------------------------------------------**/
#include "nftask.h"

/** Private typedef ---------------------------------------------------------**/

typedef struct Item{
     NFTASK_Function        Function;           /**< \brief 关联的函数 */
     uint32_t               TimeLable;          /**< \brief 时标 */
     uint32_t               TimeLableMax;       /**< \brief 时标最大值 */
     struct Item           *NextItem;           /**< \brief 单链表指针 */
}Item;

typedef Item* Items;

/** Private define ----------------------------------------------------------**/

/** Private macro -----------------------------------------------------------**/
/** Private variables -------------------------------------------------------**/

static Items itemsEndless = NULL;       /* 无限循环执行列表 */
static Items itemsTiming = NULL;        /* 定时执行列表 */
static Items itemsTimingInt = NULL;     /* 定时中断执行列表 */

static BOOLEAN timeUp;      /* TIMER溢出时标记 供NFTASK_Run()使用 */

/** Private functions -------------------------------------------------------**/
void excuteFunctions (Items items, BOOLEAN direct);

/**
* \brief 每隔(NFCONFIG_NFTASK_TIMESLICE)us中断,执行一次的中断服务程序
*/
void NFTASK_Isr(void)
{
    /* 溢出标志 供Run()使用 */
    timeUp = 1;

    excuteFunctions(itemsTimingInt,0);

}


/**
* \brief 向执行列表 添加项目 以项目中的函数指针作为每一项目的关键字
*
* \param[in]        items           执行列表
* \param[in]        setupStruct     设置结构体
*
* \retval false     失败
* \retval ture      成功
*/
static
BOOLEAN item_add (Items *items, NFTASK_SetupTypeDef *setupStruct)
{
    Item *ptr;

    /* 单位换算到us*时间us/每时标长度 */
    uint32_t timeLableMax   =   setupStruct->NFTASK_Time_Unit   *
                                setupStruct->Time               /
                                NFTASK_TIMESLICE;

    /* 第一次使用 */
    if(*items == NULL){

        /* 列表中第一个项目为空项目 */
        *items = (Item*) malloc(sizeof(Item));
        if (*items == NULL)
            return FALSE;

        /* 启动定时器 */
        NFTASK_TimerInit();
    }

    /* 查重 */
    ptr = *items;
    while(ptr->NextItem !=NULL){
        ptr = ptr->NextItem;
        if(ptr->Function == setupStruct->Function){

            /* 找到，直接处理 */
            ptr->TimeLableMax = timeLableMax;
            ptr->TimeLable = 0;
            return TRUE;
        }

    }

    /* 未找到,添加新项目 */
    ptr->NextItem = (Item*) malloc( sizeof(Item) );
    ptr = ptr->NextItem;
    if (ptr == NULL)
        return FALSE;

    /* 填充新项目 */
    ptr->Function = setupStruct->Function;
    ptr->TimeLableMax = timeLableMax;
    ptr->TimeLable = 0;
    ptr->NextItem = NULL;

    return TRUE;
}

/**
* \brief
*
* \param[in]        items
* \param[in]        pfunc    以函数指针确定项目
*
* \retval false     失败
* \retval ture      成功
*/
BOOLEAN item_del (Items items, NFTASK_Function pfunc)
{
    Item *ptr = items;
    Item *tmpPtr;

    if(pfunc == NULL)
        return FALSE;

    do{
        if(ptr->NextItem->Function == pfunc){
            /* 删除链表中该项 */
            tmpPtr = ptr->NextItem->NextItem;
            free(ptr->NextItem);
            ptr->NextItem = tmpPtr;

            return TRUE;
        }
    } while(ptr->NextItem != NULL);

    return FALSE;
}

/**
* \brief 执行函数
*
* \param[in]        items
* \param[in]        direct    是否无视时标,直接执行
*/
void excuteFunctions (Items items, BOOLEAN direct)
{
    static Item *ptr;

    if(items == NULL || items->NextItem == NULL)
         return;

    ptr = items;

    if(direct){
        while(ptr->NextItem != NULL){
            ptr = ptr->NextItem;
            (*(ptr->Function)) ();    /* 执行函数 */
        }
    }
    else{
        while(ptr->NextItem != NULL){
            ptr = ptr->NextItem;

            /* 时标自加 */
            (ptr->TimeLable)++;

            /* 时标达到预设定标值 */
            if(ptr->TimeLable == ptr->TimeLableMax){
                ptr->TimeLable = 0;
                (*(ptr->Function)) ();    /* 执行函数 */
            }
        }
    }
}

/**
* \brief
*
* \param[in]        NFTASK_SetupStruct        设置结构体 \ref NFTASK_InitTypeDef
* \param[in]        NFTASK_TYPE               TASK类型 \ref NFTASK_Type_Enum
*
* \retval false     失败
* \retval ture      成功
*/
BOOLEAN NFTASK_Setup (  NFTASK_SetupTypeDef  *NFTASK_SetupStruct,
                        NFTASK_Type_Enum      NFTASK_TYPE   )
{
    BOOLEAN retval;

    if( NFTASK_SetupStruct              == NULL ||
        NFTASK_SetupStruct->Function    == NULL ){
       return 0;
    }

    switch(NFTASK_TYPE){
        case NFTASK_TYPE_ENDLESS:
            retval = item_add(&itemsEndless,NFTASK_SetupStruct);
            break;
        case NFTASK_TYPE_TIMING:
            retval = item_add(&itemsTiming,NFTASK_SetupStruct);
            break;
        case NFTASK_TYPE_TIMINGINT:
            retval = item_add(&itemsTimingInt,NFTASK_SetupStruct);
            break;
        default:
            retval = FALSE;
    }

    return retval;
}



/**
* \brief 删除配置, 与NFTASK_Setup()作用相反
*
* \param[in]        pfunc        函数指针
* \param[in]        NFTASK_TYPE    TASK类型 \ref NFTASK_Type_Enum
*
* \retval false     失败
* \retval ture      成功
*/
BOOLEAN NFTASK_SetupDelete (NFTASK_Function pfunc, NFTASK_Type_Enum NFTASK_TYPE)
{
    BOOLEAN retval;  /* 返回值 */

    switch(NFTASK_TYPE){
        case NFTASK_TYPE_ENDLESS:
            retval = item_del(itemsEndless, pfunc);
            break;
        case NFTASK_TYPE_TIMING:
            retval = item_del(itemsTiming, pfunc);
            break;
        case NFTASK_TYPE_TIMINGINT:
            retval = item_del(itemsTimingInt, pfunc);
            break;
        default:
            retval = 0;
    }

    return retval;
}

/**
* \brief Run子程序
* \details 如果需要用到 无限循环执行/定时执行 功能
*          则需要在主循环中调用该子程序
*/
void NFTASK_Run(void)
{
    if(timeUp){
        timeUp = 0;
        excuteFunctions(itemsTiming,0);
    }

    excuteFunctions(itemsEndless,1);

}

/**END OF FILE**/
