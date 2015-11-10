/**
* \file main.c
* \brief 部件 NKEYSCAN 实例
*
* \internal
* \par Modification history
* - 1.00 2015-11-08 noodlefighter, first implementation
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
    NFDEBUG_("DEMO OF COMPONENT NKEYSCAN");
    NFDEBUG_("");
    NFDEBUG_("Link your 8pin matrix keyboard to PORTA pin0-7");
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
