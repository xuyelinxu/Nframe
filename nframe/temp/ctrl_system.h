#ifndef __CTRL_SYSTEM_H
#define __CTRL_SYSTEM_H

#include "stm32f10x.h"

typedef enum {
    CTRL_MSG_TYPE_SYS_NONE,
    CTRL_MSG_TYPE_SYS_ENTRY,  //״̬�����¼�
    CTRL_MSG_TYPE_SYS_QUIT,   //״̬�˳��¼� msg=��Ҫ������¼�

    CTRL_MSG_TYPE_DEBUG_STRING, //DEBUG�ı�����

    CTRL_MSG_TYPE_KEYBOARD_MSG, //��������
    CTRL_MSG_TYPE_MPU6050_DATA, //MPU6050����

}CTRL_MSG_TYPE;

typedef struct{
    CTRL_MSG_TYPE MsgType;
    uint32_t Msg;
}CTRL_MSG;

void CTRL_msgIn(CTRL_MSG *msg,uint8_t immediately);
void CTRL_dealQueueMsg(void);
#endif // __CTRL_SYSTEM_H
