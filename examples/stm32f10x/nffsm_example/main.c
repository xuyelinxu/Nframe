/**
* \file main.c
* \brief nffsm stm32f10x示例
*        FSM即Finite State Machine, 有限状态机
*        用状态机模拟一把只能打2发的手枪(有这样的东西吗? 我也不知道)
*        有3个状态: 剩余2颗子弹(b2) / 1颗(b1) / 无子弹((b0)
*        用户向串口传入"shot#"时, 剩有子弹时传回"POM!"; 无子弹时传回"KA.."
*        用户向串口传入"load#"时, 填充子弹, 传回"load complete."
*
*                                               <------------
*                                              |             |
*           b2 ---(shot)---> b1 ---(shot)---> b0  ---(shot)---
*           |                 |                |
*           ^              (load)           (load)
*           |                 |                |
*            --------<----------------<--------
*
*        NFFSM_Init()           初始化NFFSM
*        NFFSM_StateTran()      状态转换
*        NFFSM_MsgIn()          消息传入
*
*        NFFSM中 一个函数表示一个状态,这里以gun_b2这个状态为例
*        void gun_b2(uint8_t msgType ,void *msg);
*        msgType为消息类型, 具体值由用户定义 但存在两个特殊值:
*        NFFSM_MSGTYPE_STATE_INTO(0xFE), NFFSM_MSGTYPE_STATE_OUT(0xFF)
*        分别用来表示状态进入和退出
*        他们的值也说明了, 用户可以定义253个消息类型
*
*        P.S.这里使用了NFDEBUG模块的命令解析功能 建议先阅读相关例程
* \internal
* \par Modification history
* - 1.00 2015-10-23 noodlefighter, first implementation
* \endinternal
*/

#include "stm32f10x.h"
#include "nframe.h"

/* 消息传入 这里使用了NFDEBUG模块 */
void msg_shot (uint8_t ParmCount, char **Parm);
void msg_load (uint8_t ParmCount, char **Parm);

/* 用三种状态表示剩余子弹数量 */
void gun_b2(uint8_t msgType ,void *msg);
void gun_b1(uint8_t msgType ,void *msg);
void gun_b0(uint8_t msgType ,void *msg);


/**< \brief DEBUG命令配置 */
NFDEBUG_COMMAND_LIST(2)
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
    NFFSM_MsgIn(&gun, GUN_MSG_SHOT, 0);
}

void msg_load (uint8_t ParmCount, char **Parm)
{
    NFFSM_MsgIn(&gun, GUN_MSG_LOAD, 0);
}

/* FSM 各状态的消息处理程序 */
void gun_b2(uint8_t msgType ,void *msg)
{
    switch(msgType){
        case NFFSM_MSGTYPE_STATE_INTO:
            NFDEBUG_("load complete.");         /* 状态转入b2 表示装弹 */
            NFDEBUG_("2 bullets left");
            NFDEBUG_("");
            break;
        case NFFSM_MSGTYPE_STATE_OUT:
            break;
        case GUN_MSG_SHOT:
            NFDEBUG_("POM!");                   /* 开枪 */
            NFFSM_StateTran(&gun, gun_b1);      /* 子弹减少 状态转至b1 */
            break;
        case GUN_MSG_LOAD:
            NFFSM_StateTran(&gun, gun_b2);      /* 装弹 状态转至b2 */
            break;
        default:
            break;
    }
}

void gun_b1(uint8_t msgType ,void *msg)
{
    switch(msgType){
        case NFFSM_MSGTYPE_STATE_INTO:
            NFDEBUG_("1 bullets left");         /* 状态转入b1 表示剩余1颗子弹 */
            NFDEBUG_("");
            break;
        case NFFSM_MSGTYPE_STATE_OUT:
            break;
        case GUN_MSG_SHOT:
            NFDEBUG_("POM!");                   /* 开枪 */
            NFFSM_StateTran(&gun, gun_b0);      /* 子弹减少 状态转至b0 */
            break;
        case GUN_MSG_LOAD:
            NFFSM_StateTran(&gun, gun_b2);      /* 装弹 状态转至b2 */
            break;
        default:
            break;
    }
}

void gun_b0(uint8_t msgType ,void *msg)
{
    switch(msgType){
        case NFFSM_MSGTYPE_STATE_INTO:
            NFDEBUG_("No bullets left");        /* 状态转入b1 表示无剩余子弹 */
            NFDEBUG_("");
            break;
        case NFFSM_MSGTYPE_STATE_OUT:
            break;
        case GUN_MSG_SHOT:
            NFDEBUG_("KA..");                   /* 开枪...没打出子弹 */
            NFDEBUG_("");
            break;
        case GUN_MSG_LOAD:
            NFFSM_StateTran(&gun, gun_b2);      /* 装弹 状态转至b2 */
            break;
        default:
            break;
    }
}

/**
* \brief 主程序
*/
int main(void)
{
    NFRAME_Init();      /* NFrame初始化 */

    NFDEBUG_("");
    NFDEBUG_("DEMO OF NFRAME NFFSM MODULE");
    NFDEBUG_("");

    NFDEBUG_("Program simulate a gun with two bullets.");
    NFDEBUG_("Send \"shot#\" and \"load#\" to control it.");
    NFDEBUG_("");

    NFFSM_Init(&gun, gun_b2);   /* 初始化FSM, 初状态为b2, 剩余两颗子弹 */

    while(1)
    {
        NFRAME_Run();
    }
}

void assert_failed(uint8_t* file, uint32_t line)
{
    while (1);
}
