/**
* \file main.c
* \brief nfmsg stm32f10x示例
*
* \internal
* \par Modification history
* - 1.00 2015-10-19 noodlefighter, first implementation
* \endinternal
*/

#include "stm32f10x.h"
#include "nframe.h"

NFMSG_CallbackMsg myMsgHandlerFunc(NFMSG_MsgPackDef  *pMsgPack);
static NFMSG_MsgHandlerDef myMsgHandlers[] = { {myMsgHandlerFunc, NFMSG_TYPE_GENERAL} };
static NFMSG_MsgPointDef myMsgPoint = { 1, myMsgHandlers};

/**
* \brief 消息点 接收消息的 消息处理程序
*        接收到消息后显示消息
*        将收到的消息加一传回
*/
NFMSG_CallbackMsg myMsgHandlerFunc(NFMSG_MsgPackDef  *pMsgPack)
{
    NFMSG_CallbackMsg backMsg = *(uint16_t*)(pMsgPack->pMsg)+1;

    NFDEBUG_("myMsgHandlerFunc: %d", *(uint16_t*)(pMsgPack->pMsg));

    return backMsg;
}

/**
* \brief 发送消息子程序 回调程序
*/
void myCallbackFunc(NFMSG_CallbackMsg CallbackMsg)
{
    NFDEBUG_("myCallbackFunc: %d", CallbackMsg);
}

/**
* \brief 发送消息子程序
*/
void sendMsg(void)
{
    static uint16_t i=0;
    NFMSG_MsgPackDef pack;

    NFDEBUG_("");
    NFDEBUG_("NFMSG EXAMPLE");

    pack.pTarget = &myMsgPoint;
    pack.pfnCallback = myCallbackFunc;
    pack.pMsg = &i;
    pack.MsgSize = sizeof(NFMSG_MsgPackDef);
    pack.MsgType = NFMSG_TYPE_GENERAL;

    if(!NFMSG_SendMsg(&pack, FALSE)){
        NFDEBUG_("Send Msg Error");
    }

    /* 此处改变i的值 不会影响目标消息点 NFMSG模块在做消息传递时拷贝了一份 */
    i++;
}


/**
* \brief 主函数中调用NFTASK模块
*/
int main(void)
{
    NFTASK_SetupTypeDef taskSetup;

    NFRAME_Init();      /* NFrame初始化 */

    /* 设置每隔1s 向消息点myPointHandle  发送消息 */
    taskSetup.Function          = sendMsg;
    taskSetup.NFTASK_Time_Unit  = NFTASK_TIME_UNIT_MS;
    taskSetup.Time              = 100;
    NFTASK_Setup(&taskSetup, NFTASK_TYPE_TIMINGINT);

    while(1)
    {
        NFRAME_Run();
    }
}

void assert_failed(uint8_t* file, uint32_t line)
{
    while (1);
}
