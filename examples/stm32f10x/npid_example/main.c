/**
* \file main.c
* \brief 部件 NPID 实例....暂时没想到怎么做
*
* \internal
* \par Modification history
* - 1.00 2015-11-10 noodlefighter, first implementation
* \endinternal
*/
#include "nframe.h"

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

#include "keyboard.h"
/**
* \brief 主函数中调用NFDEBUG模块
*/
int main (void)
{
    NFRAME_Init();      /* NFrame初始化 */

    NFDEBUG_("");
    NFDEBUG_("DEMO OF COMPONENT nPID");
    NFDEBUG_("");

    KEYBOARD_Init();

    while(1)
    {
        NFRAME_Run();
    }
}


void assert_failed (uint8_t* file, uint32_t line)
{
    while (1);
}
