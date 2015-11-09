/**
* \file nftask_example.c
* \brief stm32f10x示例
*
* \internal
* \par Modification history
* - 1.00 2015-08-31 noodlefighter, first implementation
* \endinternal
*/
#include "stm32f10x.h"
#include "nframe.h"

/**
* \brief 测试子程序
*/
void test_func(void)
{
    static uint16_t i=0;
    NFDEBUG_("");
    NFDEBUG("test_func %d", i++);
}

/**
* \brief 主函数中调用NFTASK模块
*/
int main(void)
{
    NFTASK_SetupTypeDef taskSetup;

    NFRAME_Init();      /* NFrame初始化 */

    taskSetup.Function          = test_func;
    taskSetup.NFTASK_Time_Unit  = NFTASK_TIME_UNIT_MS;
    taskSetup.Time              = 1000;
    NFTASK_Add(&taskSetup, NFTASK_TYPE_TIMING);

    while(1)
    {
        NFRAME_Run();
    }
}

void assert_failed(uint8_t* file, uint32_t line)
{
    while (1);
}
