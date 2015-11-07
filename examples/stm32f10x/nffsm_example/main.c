/**
* \file main.c
* \brief nffsm stm32f10x示例
*        用状态机模拟一把只能打2发的手枪(?有这样的东西吗 我也不知道)
*        有3个状态: 剩余2颗子弹(b2) / 1颗(b1) / 无子弹((b0)
*        用户向串口传入"shot"时, 剩有子弹时传回"POM!"; 无子弹时传回"KA.."
*        用户向串口传入"load"时, 填充子弹, 传回"load complete."
*
*        P.S.这里使用了NFDEBUG模块的命令解析功能 建议先阅读相关例程
* \internal
* \par Modification history
* - 1.00 2015-10-23 noodlefighter, first implementation
* \endinternal
*/

#include "stm32f10x.h"
#include "nframe.h"

void msg_shot (uint8_t ParmCount, char **Parm);
void msg_load (uint8_t ParmCount, char **Parm);

/**< \brief DEBUG命令配置 */
NFDEBUG_CommandDef NFDEBUG_CommandList[] =
{
    {msg_shot, 4, "shot"},
    {msg_load, 4, "load"}
};

NFFSM gun;

typedef enum {
    GUN_MSG_SHOT,
    GUN_MSG_LOAD,
} GUN_MSG;

void msg_shot (uint8_t ParmCount, char **Parm)
{
    NFFSM_MsgIn(&gun, (void*)GUN_MSG_SHOT);
}

void msg_load (uint8_t ParmCount, char **Parm)
{
    NFFSM_MsgIn(&gun, (void*)GUN_MSG_LOAD);
}

/* FSM 消息处理程序 */
void gun_b2(NFFSM_MSGTYPE msgType ,void *msg)
{
    if(msgType == NFFSM_MSGTYPE_STATE_INTO){
        NFDEBUG_("load complete.");
        NFDEBUG_("2 bullets left");
    }
    else if(msgType == NFFSM_MSGTYPE_USERMSG){
        if((msg == (void*)GUN_MSG_SHOT)){
            NFDEBUG_("POM!");
            NFFSM_StateTran(&gun, gun_b2);
        }
        else if(msg == (void*)GUN_MSG_LOAD){
            NFFSM_StateTran(&gun, gun_b2);
        }
    }
}

void gun_b1(NFFSM_MSGTYPE msgType ,void *msg)
{
    if(msgType == NFFSM_MSGTYPE_STATE_INTO){
        NFDEBUG_("1 bullets left");
    }
    else if(msgType == NFFSM_MSGTYPE_USERMSG){
        if(msg == (void*)GUN_MSG_SHOT){
            NFDEBUG_("POM!");
            NFFSM_StateTran(&gun, gun_b2);
        }
        else if(msg == (void*)GUN_MSG_LOAD){
            NFFSM_StateTran(&gun, gun_b2);
        }
    }
}

void gun_b0(NFFSM_MSGTYPE msgType ,void *msg)
{
    if(msgType == NFFSM_MSGTYPE_STATE_INTO){
        NFDEBUG_("No bullets left");
    }
    else if(msgType == NFFSM_MSGTYPE_USERMSG){
        if(msg == (void*)GUN_MSG_SHOT){
            NFDEBUG_("KA..");
        }
        else if(msg == (void*)GUN_MSG_LOAD){
            NFFSM_StateTran(&gun, gun_b2);
        }
    }
}

/**
* \brief
*/
int main(void)
{ 
    NFRAME_Init();      /* NFrame初始化 */

    NFFSM_Init(&gun, gun_b2);

    while(1)
    {
        NFRAME_Run();
    }
}

void assert_failed(uint8_t* file, uint32_t line)
{
    while (1);
}
