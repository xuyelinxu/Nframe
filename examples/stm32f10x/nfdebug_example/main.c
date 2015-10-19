/**
* \file main.c
* \brief nfdebug模块 stm32f10x示例
*
* \internal
* \par Modification history
* - 1.00 2015-10-16 noodlefighter, first implementation
* \endinternal
*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "nframe.h"

void Command_test (uint8_t ParmCount, char **Parm);

/**< \brief DEBUG命令配置 */
NFDEBUG_CommandDef NFDEBUG_CommandList[] =
{
    {Command_test, 5, "test "}
};

/**
* \brief 主函数中调用NFDEBUG模块
*/
int main (void)
{
    uint8_t i;

    NFRAME_Init();      /* NFrame初始化 */

    NFDEBUG_("");
    NFDEBUG_("DEMO OF NFRAME DEBUG MODULE");
    NFDEBUG_("Ouput Test Start");
    NFDEBUG_("");

    for(i=0; i<10; i++){
        NFDEBUG("i=%d", i);
    }

    NFDEBUG_("");
    NFDEBUG_("Ouput Test End");
    NFDEBUG_("");

    NFDEBUG_("Now, you can try to input command: \"test parm1 parm2#\"");
    NFDEBUG_("It will return all parm.");

    while(1)
    {
        NFRAME_Run();
    }
}

/**
* \brief test命令 处理函数
*        功能: 返回所有参数
*/
void Command_test (uint8_t ParmCount, char **Parm)
{
    uint8_t i=0;

    NFDEBUG_("");
    NFDEBUG_("Command_test:");

    while(i < ParmCount){
        NFDEBUG_("Parm%d: %s", i, Parm[i]);
        i++;
    }
}

void assert_failed (uint8_t* file, uint32_t line)
{
    while (1);
}
