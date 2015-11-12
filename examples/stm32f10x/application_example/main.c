/**
* \file     main.c
* \brief    使用NFRAME 狗将平面状态机程序的 完整应用例子
*
* \internal
* \par Modification history
* - 1.00 2015-11-11 noodlefighter, first implementation
* \endinternal
*/
#include "nframe.h"

#include "mainfsm.h"

int main (void)
{
    NFRAME_Init();      /* NFrame初始化 */

    NFDEBUG_("");
    NFDEBUG_("NFRAME APPLICATION DEMO");
    NFDEBUG_("Link your 8pin matrix keyboard to PORTA pin0-7");
    NFDEBUG_("");

    MainFsm_Startup();

    while(1)
    {
        NFRAME_Run();
    }
}


void assert_failed (uint8_t* file, uint32_t line)
{
    while (1);
}
