#include <string.h>
#include <math.h>
#include "delay.h"

#include "ctrl_system.h"
#include "my_clock.h"
#include "12864.h"
#include "keyboard.h"
#include "beep.h"
#include "motor_control.h"
#include "pwm4.h"
#include "circle.h"

#include "interface_mpu6050.h"
#include "PID_BYKX.h"

#include "debug.h"

/*ϵͳ״̬ö��*/
typedef enum SystemState{
    NONE,                   //��״̬
    INIT,                   //�ȴ�����

    STATE_J1,               //����1

    STATE_J2,               //����2 �������� ���ó���
    STATE_J2_WORKING,       //�������

    STATE_J3,               //����3
    STATE_J3_WORKING,       //�������

    STATE_J4,               //����4

    STATE_F,                //���Ӳ���
    STATE_F_WORKING,        //�������

    STATE_EX,               //����

    /**
     DEBUGģʽ
        ����: mpu6050,
     */
    STATE_DEBUG,

    STATE_UNKNOW,   //δ֪״̬
}SystemState;

/* ���� */

/** DATA_J1 ************************************************* */
#define DATA_J1_PID_X_P         800
#define DATA_J1_PID_X_I         0
#define DATA_J1_PID_X_D         0
#define DATA_J1_PID_Y_P         200
#define DATA_J1_PID_Y_I         50
#define DATA_J1_PID_Y_D         0
#define DATA_J1_GOAL            24

static struct {
    /* 2·PID */
    PID_Type pid_data_x;
    PID_Type pid_data_y;

    uint8_t show;           /* �Ƿ���ʾ������Ϣ */
    uint8_t show_timelabel; /* ��ʾ�������� */

    uint16_t goal;          /* ��Ŀ��Ƕ� */
} data_j1;      /* ����1 �����ڶ� */

/** DATA_J2 ************************************************* */
#define DATA_J2_PID_X_P 800
#define DATA_J2_PID_X_I 0
#define DATA_J2_PID_X_D 0
#define DATA_J2_PID_Y_P 200
#define DATA_J2_PID_Y_I 0
#define DATA_J2_PID_Y_D 0
#define DATA_J2_ANGLE_TABLE_NUMBER (((60-30)/5)+1)      /* 7 */

/* pidĿ��ǶȲ��Ԥ��ֵ ����Ϊ5cm */
const uint8_t DATA_J2_angle_table_30to60 [DATA_J2_ANGLE_TABLE_NUMBER] = {
    12,   /* 30 */
    14,   /* 35 */
    16,   /* 40 */
    19,   /* 45 */
    21,   /* 50 */
    23,   /* 55 */
    26,   /* 60 */
};

static struct {
    /* 2·PID */
    PID_Type pid_data_x;
    PID_Type pid_data_y;

    uint8_t show;           /* �Ƿ���ʾ������Ϣ */
    uint8_t show_timelabel; /* ��ʾ�������� */

    uint8_t length;         /* ���õĳ��� */

    /* ��ǰĿ��ǶȲ�� �ɱ��  */
    uint16_t angle_table_30to60[DATA_J2_ANGLE_TABLE_NUMBER];
} data_j2;      /* ����2 */

static __INLINE
float DATA_J2_angle_table_value_get(uint8_t length)
{
    float rev,pre,last;
    int index = length / 5 - 6;
    int mod = length % 5;
    if(mod == 0){
        /* 5�ı��� */
        rev = (float)((data_j2.angle_table_30to60)[index]);
    }
    else {
        /* һ����� */
        pre     = (float)(data_j2.angle_table_30to60[index]);
        last    = (float)(data_j2.angle_table_30to60[index+1]);
        rev     = (float)( last + (last - pre) * mod / 5 );
    }
    return rev;
}

/** DATA_J3 ************************************************* */

/* pwm_x,pwm_y,goal_angel*/
const uint32_t DATA_J3_pwm_table[][3] = {
    { 14000 , 0     , 7},       /* 0  */
    { 14000 , 5000  , 7},       /* 10 */
    { 12000 , 6000  , 7},       /* 20 */
    { 13000 , 9000  , 7},       /* 30 */
    { 13000 , 12000 , 7}        /* 40 */
};

static struct {
    uint8_t show;           /* �Ƿ���ʾ������Ϣ */
    uint8_t show_timelabel; /* ��ʾ�������� */

    uint8_t angle;          /* ���õĽǶ� */

    uint32_t pwm_x;         /* x��pwm�̶�ֵ */
    uint32_t pwm_y;         /* y��pwm�̶�ֵ */
    uint8_t goal_angle;       /* ?��Ŀ��� ��������״̬ */
    uint8_t x_or_y_angle;   /* x��Ϊ0 y��Ϊ1 */
    uint8_t angle_plus_minus;   /* �Ƕ��жϹ��� */
} data_j3;      /* ����3 */

static __INLINE
void DATA_J3_pwm_values_set(uint8_t angle)
{
    if(angle <= 40){     /* 0-40�� */
        angle = angle / 10;
        data_j3.pwm_x       = DATA_J3_pwm_table[angle][0];
        data_j3.pwm_y       = DATA_J3_pwm_table[angle][1];
        data_j3.goal_angle  = DATA_J3_pwm_table[angle][2];
        data_j3.x_or_y_angle = 0;
        data_j3.angle_plus_minus = 0;
    }
    else if(angle <= 90)   /* 50-90 */
    {
        angle = (90 - angle) / 10;
        data_j3.pwm_x       = DATA_J3_pwm_table[angle][1];
        data_j3.pwm_y       = DATA_J3_pwm_table[angle][0];
        data_j3.goal_angle  = DATA_J3_pwm_table[angle][2];
        data_j3.x_or_y_angle = 1;
        data_j3.angle_plus_minus = 0;
    }
    else if(angle <= 130)  /* 100-130 */
    {
        angle = (angle - 90) / 10;
        data_j3.pwm_x       = DATA_J3_pwm_table[angle][1];
        data_j3.pwm_y       = -DATA_J3_pwm_table[angle][0];
        data_j3.goal_angle  = DATA_J3_pwm_table[angle][2];
        data_j3.x_or_y_angle = 1;
        data_j3.angle_plus_minus = 1;
    }
    else if(angle < 180)  /* 140-180 */
    {
        angle = (180 - angle) / 10;
        data_j3.pwm_x       = DATA_J3_pwm_table[angle][0];
        data_j3.pwm_y       = -DATA_J3_pwm_table[angle][1];
        data_j3.goal_angle  = DATA_J3_pwm_table[angle][2];
        data_j3.x_or_y_angle = 0;
        data_j3.angle_plus_minus = 0;
    }

}
/** DATA_J4 ************************************************* */
#define DATA_J4_PID_X_P 2000
#define DATA_J4_PID_X_I 0
#define DATA_J4_PID_X_D 0
#define DATA_J4_PID_Y_P 2000
#define DATA_J4_PID_Y_I 0
#define DATA_J4_PID_Y_D 0
#define DATA_J4_NOSTOPVALUE 0

static struct {
    /* 2·PID */
    PID_Type pid_data_x;
    PID_Type pid_data_y;
    uint8_t show;           /* �Ƿ���ʾ������Ϣ */
    uint8_t show_timelabel; /* ��ʾ�������� */

    uint32_t insideTime;   /* ����ͳ���Ƿ����Ȧ�� */
    uint32_t completeTime;   /* ����ͳ���Ƿ���� */
    uint8_t alreadyComplete;
} data_j4;      /* ����4 ��ֹ����ʹ� */

/** DATA_F ************************************************* */

/* ��PWMֵ, ��ʱʱ�� */
const uint32_t DATA_F_table[][2] = {
    { 21400 , 4     },       /* 15 */
    { 15600 , 4     },       /* 20 */
    { 12800 , 5     },       /* 25 */
    { 14000 , 5     },       /* 30 */
    { 16000 , 4     }        /* 35 */
};

static struct {
    uint8_t show;           /* �Ƿ���ʾ������Ϣ */
    uint8_t show_timelabel; /* ��ʾ�������� */

    uint32_t height;        /* ���и߶� */

    /* ����÷��г�����Ҫ�Ĳ��� */
    uint32_t totalPWM;
    uint16_t delay_ms;

} data_f;      /* ����4 ��ֹ����ʹ� */


static __INLINE
void DATA_F_setValue_from_table (uint8_t d)
{
    /* ������dΪֱ�� */
//    if(d % 5 == 0)
//    {
//        data_f.totalPWM = DATA_F_table[d / 5 - 3][0];
//        data_f.delay_ms = DATA_F_table[d / 5 - 3][1];
//    }
//    else{
//        data_f.delay_ms = DATA_F_table[d / 5 - 3][1];
//
//        uint32_t pre    = DATA_F_table[d / 5 - 3][0];
//        uint32_t last   = DATA_F_table[d / 5 - 2][0];
//        data_f.totalPWM = last +  (last-pre) * (d % 5) / 5 ;
//    }
        data_f.totalPWM = DATA_F_table[d / 5 - 3][0];
        data_f.delay_ms = DATA_F_table[d / 5 - 3][1];
}

/** DATA_DEBUG ************************************************* */
#define DATA_DEBUG_GOALVALUE 0
//static struct {
//
//} data_debug = {0};

/** DATA_GOBAL ************************************************* */
static struct {
    uint8_t switch_show_mpu6050;
} data_gobal = {0};

/*��ǰϵͳ״̬*/
SystemState systemState = NONE;


/**/
void dealMsg(CTRL_MSG *msg);

/*��Ϣ����*/
#define MSG_QUEUE_LENGTH 50
CTRL_MSG msgQueue[MSG_QUEUE_LENGTH];
uint8_t msgQueueStartIndex = 0;
uint8_t msgQueueEndIndex = 0;

#define msgQueueIsEmpty (msgQueueStartIndex == msgQueueEndIndex)
#define msgClearQueue() (msgQueueStartIndex = msgQueueEndIndex = 0)

uint8_t msgQueueAdd(CTRL_MSG *msg){
    uint8_t backup = msgQueueEndIndex;
    msgQueue[msgQueueEndIndex] = *msg;

    if(++msgQueueEndIndex >= MSG_QUEUE_LENGTH)   //Array tail
        msgQueueEndIndex = 0;

    if(msgQueueEndIndex  == msgQueueStartIndex){ //Queue full
        msgQueueEndIndex = backup;
        return 0;
    }

    return 1;
}

CTRL_MSG msgQueueDequeue(void){
    uint8_t backup = msgQueueStartIndex;
    msgQueueStartIndex++;
    if(msgQueueStartIndex >= MSG_QUEUE_LENGTH){
        msgQueueStartIndex = 0;
    }

    return msgQueue[backup];
}

static __INLINE void msgQueueClear(void){
    msgQueueStartIndex = 0;
    msgQueueEndIndex = 0;
}

//״̬ת��ʱ����
void stateChange(SystemState target){
    if(target == STATE_UNKNOW) return;  //δ֪״̬,��ת��
    if(systemState == target) return;   //��ת��,������

    #ifdef DEBUG
    printf("\nState Change: %d -> %d", systemState, target);
    #endif // DEBUG

//    char buf[20];

    //�˳�״̬ �¼�
    switch(systemState){
    case NONE:
        DEBUG_Write("\nHardware Initing...");

        DEBUG_Write("\nLCD Initing...");
        LCD12864_Init();        //Һ����ʼ��
        LCD12864_Clear_TXT();
        LCD12864_Disp_HZ(0,"���ڳ�ʼ��������",8);
        DEBUG_Write("Complete");

        BEEP_Init();    //��������ʼ��

        DEBUG_Write("\nKeyboard Initing...");
        KEY_init();         //������ʼ��
        DEBUG_Write("Complete");

        DEBUG_Write("\nMotor Initing...");
        MOTOR_Init();
        DEBUG_Write("Complete");

        DEBUG_Write("\nMPU6050 Initing...");
        MPU6050INTERFACE_Init(2);       /* 500Hz */
        DEBUG_Write("Complete");
        LCD12864_Disp_HZ(0,"��ʼ����Ͽ�ʼ��",8);

        /* ���ȫ�ֱ��� */
        for (int i=0 ; i<DATA_J2_ANGLE_TABLE_NUMBER ; i++)
            data_j2.angle_table_30to60[i] = DATA_J2_angle_table_30to60[i];


        #ifdef DEBUG
        printf("\nINIT Complete!");
        #endif // DEBUG
        break;
    case STATE_F_WORKING:
        CIRCLE_DeInit();
        break;
    case STATE_EX:
        CIRCLE_DeInit();
        break;
    case INIT:
        MYCLOCK_Init(); //��ʼ��ʱ
        break;
    default:
        break;
    }

    //����״̬ �¼�
    systemState = target;
    switch(systemState){
    case INIT:
        LCD12864_Clear_TXT();
        LCD12864_Disp_HZ(0,"��ѡ����ģʽ��",8);
        LCD12864_Disp_HZ(1,"1-4,����        ",8);
        LCD12864_Disp_HZ(2,"5,����  6,����  ",8);
        LCD12864_Disp_HZ(3,"9,DEBUG         ",8);
        MOTOR_AllStop();
        break;
    case STATE_J1:
        LCD12864_Clear_TXT();
        LCD12864_Disp_HZ(0,"����1           ",8);
        LCD12864_Disp_HZ(2,"15��������ɰڶ�",8);
        LCD12864_Disp_HZ(3,"������50cmֱ�߶�",8);

        /* �ֲ�������ʼ�� */
        data_j1.goal = DATA_J1_GOAL;

        /* ��·pid��ʼ�� */
        PID_Init(DATA_J1_PID_X_P, DATA_J1_PID_X_I, DATA_J1_PID_X_D, 60000, -60000, &(data_j1.pid_data_x));
        PID_Init(DATA_J1_PID_Y_P, DATA_J1_PID_Y_I, DATA_J1_PID_Y_D, 60000, -60000, &(data_j1.pid_data_y));

        /* СС�ĳ��ٶ� */
        MOTOR_SetX(8000,0);
        delay_ms(1000);
        MOTOR_SetX(0,0);
        break;
    case STATE_J2:
        LCD12864_Clear_TXT();
        LCD12864_Disp_HZ(0,"����2           ",8);
        LCD12864_Disp_HZ(1,"�Ӿ�ֹ��ʼ      ",8);
        LCD12864_Disp_HZ(2,"���趨����ֱ�߶�",8);
        LCD12864_Disp_HZ(3,"���ȣ�0       cm",8);

        data_j2.length = 0;
        break;
    case STATE_J2_WORKING:
        LCD12864_Disp_HZ(0,"����2     ������",8);

        PID_Init(DATA_J2_PID_X_P, DATA_J2_PID_X_I, DATA_J2_PID_X_D, 60000, -60000, &(data_j2.pid_data_x));
        PID_Init(DATA_J2_PID_Y_P, DATA_J2_PID_Y_I, DATA_J2_PID_Y_D, 60000, -60000, &(data_j2.pid_data_y));
        break;
    case STATE_J3:
        LCD12864_Clear_TXT();
        LCD12864_Disp_HZ(0,"����3           ",8);
        LCD12864_Disp_HZ(1,"15s�������ɰڶ� ",8);
        LCD12864_Disp_HZ(2,"������50cmֱ�߶�",8);
        LCD12864_Disp_HZ(3,"�Ƕȣ�0       ��",8);
        data_j3.angle = 0;
        break;
    case STATE_J3_WORKING:
        LCD12864_Disp_HZ(0,"����3     ������",8);
        /* СС�ĳ��ٶ� */
        MOTOR_SetX(data_j3.pwm_x,0);
        MOTOR_SetY(data_j3.pwm_y,0);
        delay_ms(8000);
        MOTOR_SetX(0,0);
        break;
    case STATE_J4:
        LCD12864_Clear_TXT();
        LCD12864_Disp_HZ(0,"����4           ",8);
        LCD12864_Disp_HZ(2,"����һ���Ƕȷſ�",8);
        LCD12864_Disp_HZ(3,"5s�ڴﵽ��ֹ״̬",8);

        /* ��·pid��ʼ�� */
        PID_Init(DATA_J4_PID_X_P, DATA_J4_PID_X_I, DATA_J4_PID_X_D, 20000, -20000, &(data_j4.pid_data_x));
        PID_Init(DATA_J4_PID_Y_P, DATA_J4_PID_Y_I, DATA_J4_PID_Y_D, 20000, -20000, &(data_j4.pid_data_y));

        data_j4.alreadyComplete = 0;
        break;
    case STATE_F:
        LCD12864_Clear_TXT();
        LCD12864_Disp_HZ(0,"����            ",8);
        LCD12864_Disp_HZ(1,"����Բ��        ",8);
        LCD12864_Disp_HZ(3,"�뾶��        cm",8);

        data_f.height = 0;
        break;
    case STATE_F_WORKING:
        LCD12864_Disp_HZ(0,"����      ������",8);
        break;
    case STATE_EX:
        LCD12864_Clear_TXT();
        LCD12864_Disp_HZ(0,"����            ",8);
        LCD12864_Disp_HZ(1,"����������      ",8);
        CIRCLE_Init(25000, 5);  //������� ���������Σ�����
        break;
    case STATE_DEBUG:
        LCD12864_Clear_TXT();
        LCD12864_Disp_HZ(0,"DEBUG_MODE      ",8);
        #ifdef DEBUG
        printf("\nDEBUG_MODE");
        #endif
        break;
    default:
        break;
    }

    msgQueueClear();
}

void CTRL_msgIn(CTRL_MSG *msg,uint8_t immediately){
    #ifdef DEBUG
    //printf("\n CTRL_msgIn type=%X,msg=%X",msg->MsgType,msg->Msg);
    #endif // DEBUG

    if(immediately){
        dealMsg(msg);
    }else{
        msgQueueAdd(msg);
    }
}

void dealMsg_INIT(CTRL_MSG_TYPE MsgType,uint32_t msg)
{
    if(MsgType == CTRL_MSG_TYPE_KEYBOARD_MSG){
        switch(msg){
        case 1:
            stateChange(STATE_J1);
            break;
        case 2:
            stateChange(STATE_J2);
            break;
        case 3:
            stateChange(STATE_J3);
            break;
        case 4:
            stateChange(STATE_J4);
            break;
        case 5:
            stateChange(STATE_F);
            break;
        case 6:
            stateChange(STATE_EX);
            break;
        case 9:
            stateChange(STATE_DEBUG);
            break;
        }
    }
}

void dealMsg_J1(CTRL_MSG_TYPE MsgType,uint32_t msg)
{
    if(MsgType == CTRL_MSG_TYPE_MPU6050_DATA){
        /* ���� */
        MPU6050INTERFACE_Data_Struct *data =
            (MPU6050INTERFACE_Data_Struct*) msg;
        int16_t pidX,pidY,myGoal;
        myGoal = (data->pitch > 0) ? (data_j1.goal) : -(data_j1.goal) ;

        pidX = PID_Calc(myGoal,
                        data->pitch,
                        PID_CALCMODE_PID,
                        &data_j1.pid_data_x);
        pidY = PID_Calc(0,
                        data->roll,
                        PID_CALCMODE_PID,
                        &data_j1.pid_data_y);
        /* ���� */
        MOTOR_SetX(pidX, 0);
        MOTOR_SetY(pidY, 0);
        #ifdef DEBUG
        if(data_j1.show && ( (data_j1.show_timelabel++) >= 50) ){
            data_j1.show_timelabel = 0;
            printf("\npidX=%d pidY=%d pitch=%f roll=%f", pidX, pidY, data->pitch ,data->roll);
        }
        #endif
    }else if(MsgType == CTRL_MSG_TYPE_KEYBOARD_MSG){
        if(msg == KEY_MSG_CANCEL){
            stateChange(INIT);
        }
     }
     #ifdef DEBUG
    else if(MsgType == CTRL_MSG_TYPE_DEBUG_STRING){
        if(strncasecmp((const char *)msg, "setpid ", 7) == 0){
            /* PID�������� �ֱ�Ϊ CalMod Px Ix Dx Py Iy Dy */
            sscanf((const char*)msg,
                   "setpid %f %f %f %f %f %f",
                   &(data_j1.pid_data_x.Kp),
                   &(data_j1.pid_data_x.Ki),
                   &(data_j1.pid_data_x.Kd),
                   &(data_j1.pid_data_y.Kp),
                   &(data_j1.pid_data_y.Ki),
                   &(data_j1.pid_data_y.Kd));

            printf("\nPIDset: Px=%.2f Ix=%.2f Dx=%.2f Py=%.2f Iy=%.2f Dy=%.2f",
                   (data_j1.pid_data_x.Kp),
                   (data_j1.pid_data_x.Ki),
                   (data_j1.pid_data_x.Kd),
                   (data_j1.pid_data_y.Kp),
                   (data_j1.pid_data_y.Ki),
                   (data_j1.pid_data_y.Kd));
        }
        else if(strcmp((const char *)msg, "show") == 0){
            /* ��ʾ��������Ϣ���� */
            data_j1.show = !data_j1.show;
        }
        else if(strcmp((const char *)msg, "getpid") == 0){
            printf("\nPID: Px=%.2f Ix=%.2f Dx=%.2f Py=%.2f Iy=%.2f Dy=%.2f",
                   (data_j1.pid_data_x.Kp),
                   (data_j1.pid_data_x.Ki),
                   (data_j1.pid_data_x.Kd),
                   (data_j1.pid_data_y.Kp),
                   (data_j1.pid_data_y.Ki),
                   (data_j1.pid_data_y.Kd));
        }
        else if(strncasecmp((const char *)msg, "goal ",5) == 0){
            /* �趨�������Ƕ�Ŀ��ֵ */
            sscanf((const char*)msg,"goal %u", (unsigned int*)(&(data_j1.goal)) );
            printf("\nGoal Set:%u", data_j1.goal);
        }
     }
     #endif     /* DEBUG */
}

void dealMsg_J2_common(CTRL_MSG_TYPE MsgType,uint32_t msg)
{
     #ifdef DEBUG
    if(MsgType == CTRL_MSG_TYPE_DEBUG_STRING){
        if(strncasecmp((const char *)msg, "setpid ", 7) == 0){
            /* PID�������� �ֱ�Ϊ CalMod Px Ix Dx Py Iy Dy */
            sscanf((const char*)msg,
                   "setpid %f %f %f %f %f %f",
                   &(data_j2.pid_data_x.Kp),
                   &(data_j2.pid_data_x.Ki),
                   &(data_j2.pid_data_x.Kd),
                   &(data_j2.pid_data_y.Kp),
                   &(data_j2.pid_data_y.Ki),
                   &(data_j2.pid_data_y.Kd));

            printf("\nPIDset: Px=%.2f Ix=%.2f Dx=%.2f Py=%.2f Iy=%.2f Dy=%.2f",
                   (data_j2.pid_data_x.Kp),
                   (data_j2.pid_data_x.Ki),
                   (data_j2.pid_data_x.Kd),
                   (data_j2.pid_data_y.Kp),
                   (data_j2.pid_data_y.Ki),
                   (data_j2.pid_data_y.Kd));
        }
        else if(strcmp((const char *)msg, "show") == 0){
            /* ��ʾ��������Ϣ���� */
            data_j2.show = !data_j2.show;
        }
        else if(strcmp((const char *)msg, "getpid") == 0){
            printf("\nPID: Px=%.2f Ix=%.2f Dx=%.2f Py=%.2f Iy=%.2f Dy=%.2f",
                   (data_j2.pid_data_x.Kp),
                   (data_j2.pid_data_x.Ki),
                   (data_j2.pid_data_x.Kd),
                   (data_j2.pid_data_y.Kp),
                   (data_j2.pid_data_y.Ki),
                   (data_j2.pid_data_y.Kd));
        }
        else if(strncasecmp((const char *)msg, "table ", 6) == 0){
            /* �޸���ϱ����� */
            uint16_t index,value;
            sscanf((const char*)msg,
                   "table %d %d",
                   (int*)&index,
                   (int*)&value );
            data_j2.angle_table_30to60[index] = value;

            /* ����������� */
            printf("\nTable:");
            for (int i=0;i<DATA_J2_ANGLE_TABLE_NUMBER;i++)
                printf("%d ",data_j2.angle_table_30to60[i]);

        }
     }
     #endif

}

void dealMsg_J2(CTRL_MSG_TYPE MsgType,uint32_t msg)
{
     if(MsgType == CTRL_MSG_TYPE_KEYBOARD_MSG){
        if(msg == KEY_MSG_CANCEL){
            stateChange(INIT);
        }
        else if(msg == KEY_MSG_ENTER){
            /* ������� */
            if( (data_j2.length < 30) || (data_j2.length > 60)){
                /* ������� */
                LCD12864_Clear_TXT();
                LCD12864_Disp_HZ(0,"������Χ����    ",8);
                delay_ms(6000);
                stateChange(INIT);
            }
            else
                stateChange(STATE_J2_WORKING);
        }
        else if( (msg >= 1)  && (msg <= 10) ){
            /* ������������ */
            uint8_t number = (msg != 10) ? (msg) : (0) ;

            if(data_j2.length >= 10)    /* ����������λ ������������� */
                return;

            data_j2.length = (data_j2.length * 10) + number;

            /* ˢ����ʾ */
            char str[20];
            sprintf(str,"���ȣ�%2d      cm",data_j2.length);
            LCD12864_Disp_HZ(3,(const u8*)str,8);
        }
     }

}

void dealMsg_J2_WORKING(CTRL_MSG_TYPE MsgType,uint32_t msg)
{
    if(MsgType == CTRL_MSG_TYPE_KEYBOARD_MSG){
        if(msg == KEY_MSG_CANCEL){
            stateChange(INIT);
        }
    }
    else if(MsgType == CTRL_MSG_TYPE_MPU6050_DATA){
        /* ���� */
        MPU6050INTERFACE_Data_Struct *data =
            (MPU6050INTERFACE_Data_Struct*) msg;
        int16_t pidX,pidY,myGoal,goal;
        goal   = DATA_J2_angle_table_value_get(data_j2.length);
        myGoal = (data->pitch > 0) ? (goal) : (-goal) ;

        pidX = PID_Calc(myGoal,
                        data->pitch,
                        PID_CALCMODE_PID,
                        &data_j2.pid_data_x);
        pidY = PID_Calc(0,
                        data->roll,
                        PID_CALCMODE_PID,
                        &data_j2.pid_data_y);
        /* ���� */
        MOTOR_SetX(pidX,0);
        MOTOR_SetY(pidY,0);
        #ifdef DEBUG
        if(data_j2.show && ( (data_j2.show_timelabel++) >= 50) ){
            data_j2.show_timelabel = 0;
            printf("\npidX=%d pidY=%d pitch=%f roll=%f", pidX, pidY, data->pitch ,data->roll);
        }
        #endif
    }
}

void dealMsg_J3_common(CTRL_MSG_TYPE MsgType,uint32_t msg)
{
    #ifdef DEBUG
    if(MsgType == CTRL_MSG_TYPE_DEBUG_STRING){
        if(strcmp((const char *)msg, "show") == 0){
            /* ��ʾ��������Ϣ���� */
            data_j3.show = !data_j3.show;
        }
    }
    #endif

}

void dealMsg_J3(CTRL_MSG_TYPE MsgType,uint32_t msg)
{
     if(MsgType == CTRL_MSG_TYPE_KEYBOARD_MSG){
        if(msg == KEY_MSG_CANCEL){
            stateChange(INIT);
        } else if(msg == KEY_MSG_ENTER){
            /* ������� */
            if( (data_j3.angle >= 180)){
                /* ������� */
                LCD12864_Clear_TXT();
                LCD12864_Disp_HZ(0,"������Χ����    ",8);
                delay_ms(6000);
                stateChange(INIT);
            }
            else{

                /* ������ */
                DATA_J3_pwm_values_set(data_j3.angle);

                stateChange(STATE_J3_WORKING);
            }
        }
        else if( (msg >= 1)  && (msg <= 10) ){
            /* ������������ */
            uint8_t number = (msg != 10) ? (msg) : (0) ;

            if(data_j3.angle >= 100)    /* ����������λ ������������� */
                return;

            data_j3.angle = (data_j3.angle * 10) + number;

            /* ˢ����ʾ */
            char str[20];
            sprintf(str,"�Ƕȣ�%3d     ��",data_j3.angle);
            LCD12864_Disp_HZ(3,(const u8*)str,8);
        }
     }
}

void dealMsg_J3_WORKING(CTRL_MSG_TYPE MsgType,uint32_t msg)
{
    if(MsgType == CTRL_MSG_TYPE_KEYBOARD_MSG){
        if(msg == KEY_MSG_CANCEL){
            stateChange(INIT);
        }
    }
     else if(MsgType == CTRL_MSG_TYPE_MPU6050_DATA){
        /* ���� */
        MPU6050INTERFACE_Data_Struct *data =
            (MPU6050INTERFACE_Data_Struct*) msg;

        float nowAngle = (data_j3.x_or_y_angle == 0 ) ? (data->pitch) : (data->roll) ;
        int32_t motorValueX,motorValueY;

        if(data_j3.angle_plus_minus == 0){
            if((nowAngle > 0) && (nowAngle < data_j3.goal_angle)){
                motorValueX = data_j3.pwm_x;
                motorValueY = data_j3.pwm_y;
            }else if((nowAngle < 0) && (nowAngle > -(data_j3.goal_angle))) {
                motorValueX = -(data_j3.pwm_x);
                motorValueY = -(data_j3.pwm_y);
            }else{
                motorValueX = 0;
                motorValueY = 0;
            }
        }
        else{
            if((nowAngle > 0) && (nowAngle < (data_j3.goal_angle))){
                motorValueX = -data_j3.pwm_x;
                motorValueY = -data_j3.pwm_y;
            }else if((nowAngle < 0) && (nowAngle > (-data_j3.goal_angle))) {
                motorValueX = (data_j3.pwm_x);
                motorValueY = (data_j3.pwm_y);
            }else{
                motorValueX = 0;
                motorValueY = 0;
            }

        }

        /* ���� */
        MOTOR_SetX(motorValueX, 0);
        MOTOR_SetY(motorValueY, 0);

        #ifdef DEBUG
        if(data_j3.show && ( (data_j3.show_timelabel++) >= 50) ){
            data_j3.show_timelabel = 0;
            printf("\nmotorValueX=%d motorValueY=%d pitch=%f roll=%f", motorValueX ,motorValueY, data->pitch ,data->roll);
        }
        #endif
     }
}

void dealMsg_J4(CTRL_MSG_TYPE MsgType,uint32_t msg)
{
     if(MsgType == CTRL_MSG_TYPE_KEYBOARD_MSG){
        if(msg == KEY_MSG_CANCEL){
            stateChange(INIT);
        }
     }
     else if(MsgType == CTRL_MSG_TYPE_MPU6050_DATA){
        /* ���� */
        MPU6050INTERFACE_Data_Struct *data =
            (MPU6050INTERFACE_Data_Struct*) msg;
        float pidX,pidY;

        pidX = PID_Calc(0,   /* DATA_DEBUG_GOALVALUE = 0 */
                        data->pitch,
                        PID_CALCMODE_PID,
                        &data_j4.pid_data_x);

        pidY = PID_Calc(0,
                        data->roll,
                        PID_CALCMODE_PID,
                        &data_j4.pid_data_y);

        /* ���� */
        MOTOR_SetX(pidX,0);
        MOTOR_SetY(pidY,0);

        /* ��Ȧ  */
        if((fabs(data->pitch) < 12.0f ) && (fabs(data->roll) < 12.0f ))
            (data_j4.insideTime)++;
        else
            data_j4.insideTime = 0;

        if(!(data_j4.alreadyComplete) && data_j4.insideTime >= 50 ){  /* ÿ��2ms 200��400ms*/
            /* ��ɺ��PID���� */
            data_j4.pid_data_x.Kp = 1000;
            data_j4.pid_data_y.Kp = 1000;
        }

        /* ���Ȧ  */
        if((fabs(data->pitch) < 4.0f ) && (fabs(data->roll) < 4.0f ))
            (data_j4.completeTime)++;
        else
            data_j4.completeTime = 0;

        if(!(data_j4.alreadyComplete) && data_j4.completeTime >= 50 ){  /* ÿ��2ms 200��400ms*/
            /* ��ɺ��PID���� */
            data_j4.pid_data_x.Kp = 1000;
            data_j4.pid_data_y.Kp = 1000;

            LCD12864_Clear_TXT();
            LCD12864_Disp_HZ(0,"��ɣ�����������",8);

            data_j4.alreadyComplete = 1;

            BEEP_Ring(3000);
        }




     }

     #ifdef DEBUG
     else if(MsgType == CTRL_MSG_TYPE_DEBUG_STRING){
        if(strncasecmp((const char *)msg, "setpid ", 7) == 0){
            /* PID�������� �ֱ�Ϊ CalMod Px Ix Dx Py Iy Dy */
            char *pidK = (char*)msg;
            sscanf(pidK,
                   "setpid %f %f %f %f %f %f",
                   &(data_j4.pid_data_x.Kp),
                   &(data_j4.pid_data_x.Ki),
                   &(data_j4.pid_data_x.Kd),
                   &(data_j4.pid_data_y.Kp),
                   &(data_j4.pid_data_y.Ki),
                   &(data_j4.pid_data_y.Kd) );

            printf("\nPIDset: Px=%.2f Ix=%.2f Dx=%.2f Py=%.2f Iy=%.2f Dy=%.2f",
                   (data_j4.pid_data_x.Kp),
                   (data_j4.pid_data_x.Ki),
                   (data_j4.pid_data_x.Kd),
                   (data_j4.pid_data_y.Kp),
                   (data_j4.pid_data_y.Ki),
                   (data_j4.pid_data_y.Kd));
        }
        else if(strcmp((const char *)msg, "show") == 0){
            /* ��ʾ��������Ϣ���� */
            data_j4.show = !data_j4.show;
        }
        else if(strcmp((const char *)msg, "getpid") == 0){
            printf("\nPID: Px=%.2f Ix=%.2f Dx=%.2f Py=%.2f Iy=%.2f Dy=%.2f",
                   (data_j4.pid_data_x.Kp),
                   (data_j4.pid_data_x.Ki),
                   (data_j4.pid_data_x.Kd),
                   (data_j4.pid_data_y.Kp),
                   (data_j4.pid_data_y.Ki),
                   (data_j4.pid_data_y.Kd));
        }
     }
     #endif     /* DEBUG */
}



void dealMsg_F_common(CTRL_MSG_TYPE MsgType,uint32_t msg)
{
    if(MsgType == CTRL_MSG_TYPE_KEYBOARD_MSG){
        if(msg == KEY_MSG_CANCEL){
            stateChange(INIT);
        }
     }
    else if(MsgType == CTRL_MSG_TYPE_DEBUG_STRING){
        if(strcmp((const char *)msg, "show") == 0){
            /* ��ʾ��������Ϣ���� */
            data_f.show = !data_f.show;
        }
        else if(strncasecmp((const char *)msg, "circle ", 7) == 0){
            /* ���û�Բ �����ֱ�Ϊ totalPWM delay_ms */
            uint32_t totalPWM,delay_ms;

            sscanf((const char*)msg,
                   "circle %d %d",
                   &(totalPWM),
                   &(delay_ms));

            //CIRCLE_DeInit();
            CIRCLE_Init(totalPWM, delay_ms);

            printf("\nCircleSet: totalPWM=%d delay_ms=%d", totalPWM, delay_ms);
        }
    }
}

void dealMsg_F(CTRL_MSG_TYPE MsgType,uint32_t msg)
{
     if(MsgType == CTRL_MSG_TYPE_KEYBOARD_MSG){
        if(msg == KEY_MSG_CANCEL){
            stateChange(INIT);
        } else if(msg == KEY_MSG_ENTER){
            /* ������� */
            if( (data_f.height > 35) || (data_f.height < 15) ){
                /* ������� */
                LCD12864_Clear_TXT();
                LCD12864_Disp_HZ(0,"������Χ����    ",8);
                delay_ms(6000);
                stateChange(INIT);
            }
            else{
                DATA_F_setValue_from_table(data_f.height);
                CIRCLE_Init(data_f.totalPWM, data_f.delay_ms);
                stateChange(STATE_F_WORKING);
            }
        }
        else if( (msg >= 1)  && (msg <= 10) ){
            /* ������������ */
            uint8_t number = (msg != 10) ? (msg) : (0) ;

            if(data_f.height >= 100)    /* ����������λ ������������� */
                return;

            data_f.height = (data_f.height * 10) + number;

            /* ˢ����ʾ */
            char str[20];
            sprintf(str,"�뾶��%3d     cm",data_f.height);
            LCD12864_Disp_HZ(3,(const u8*)str,8);
        }
     }
}

void dealMsg_F_WORKING(CTRL_MSG_TYPE MsgType,uint32_t msg)
{

}

void dealMsg_EX(CTRL_MSG_TYPE MsgType,uint32_t msg)
{
    if(MsgType == CTRL_MSG_TYPE_KEYBOARD_MSG){
        if(msg == KEY_MSG_CANCEL){
            stateChange(INIT);
        }
    }
}

void dealMsg_DEBUG(CTRL_MSG_TYPE MsgType,uint32_t msg)
{
    #ifdef DEBUG
    if(MsgType == CTRL_MSG_TYPE_KEYBOARD_MSG){

        printf("\nKeyboad: %d", msg);

        if(msg == KEY_MSG_CANCEL){
            stateChange(INIT);
        }
    }
    #endif      /* DEBUG */
}

/* ȫ����Ϣ���� */
void dealMsg_GLOBAL(CTRL_MSG_TYPE MsgType,uint32_t msg)
{
    #ifdef DEBUG
    if(MsgType == CTRL_MSG_TYPE_MPU6050_DATA){
        MPU6050INTERFACE_Data_Struct *data =
            (MPU6050INTERFACE_Data_Struct*)msg;

        if(data_gobal.switch_show_mpu6050){
            printf("\npitch=%f roll=%f yaw=%f", data->pitch, data->roll, data->yaw);
            data_gobal.switch_show_mpu6050 = 0;
        }
    }
    else if(MsgType == CTRL_MSG_TYPE_DEBUG_STRING){
        if(strcmp((const char *)msg, "mpu6050") == 0){
            data_gobal.switch_show_mpu6050 = !data_gobal.switch_show_mpu6050;
        }
        else if(strcmp((const char *)msg, "getpwm") == 0){
            uint16_t pwms[4];
            if(PWM4_GetPWM((uint16_t*)pwms)){
                printf("\npwm1=%ud pwm2=%ud pwm3=%ud pwm4=%ud",
                       pwms[0],
                       pwms[1],
                       pwms[2],
                       pwms[3]);
            }
            else
                printf("\nGet Pwm Fail!");

        }
        else if(strncasecmp((const char *)msg, "setpwm ", 7) == 0){
            uint16_t chanN,value;
            sscanf((const char *)msg,
                   "setpwm %u %u",
                   (unsigned int*)&chanN,
                   (unsigned int*)&value);
            PWM4_SetPWM(chanN,value);
            printf("\nPWM Set!!");
        }
    }
    #endif      /* DEBUG */
}

void dealMsg(CTRL_MSG *msg){
    CTRL_MSG_TYPE MsgType = msg->MsgType;
    uint32_t msgMsg = msg->Msg;

    /* ȫ����Ϣ���� */
    dealMsg_GLOBAL(MsgType, msgMsg);

    switch(systemState){
    case NONE:
        stateChange(INIT);  //����INIT״̬
        break;
    case INIT:
        dealMsg_INIT(MsgType, msgMsg);
        break;
    case STATE_J1:
        dealMsg_J1(MsgType, msgMsg);
        break;
    case STATE_J2:
        dealMsg_J2_common(MsgType, msgMsg);
        dealMsg_J2(MsgType, msgMsg);
        break;
    case STATE_J2_WORKING:
        dealMsg_J2_common(MsgType, msgMsg);
        dealMsg_J2_WORKING(MsgType, msgMsg);
        break;
    case STATE_J3:
        dealMsg_J3_common(MsgType, msgMsg);
        dealMsg_J3(MsgType, msgMsg);
        break;
    case STATE_J3_WORKING:
        dealMsg_J3_common(MsgType, msgMsg);
        dealMsg_J3_WORKING(MsgType, msgMsg);
        break;
    case STATE_J4:
        dealMsg_J4(MsgType, msgMsg);
        break;
    case STATE_F:
        dealMsg_F_common(MsgType, msgMsg);
        dealMsg_F(MsgType, msgMsg);
        break;
    case STATE_EX:
        dealMsg_EX(MsgType, msgMsg);
        break;
    case STATE_F_WORKING:
        dealMsg_F_common(MsgType, msgMsg);
        dealMsg_F_WORKING(MsgType, msgMsg);
        break;

    case STATE_DEBUG:
        dealMsg_DEBUG(MsgType, msgMsg);
        break;
    }

    #ifdef DEBUG
    //printf("\ndealMsg: tpye=%X ,msg=%X",MsgType,msgMsg);
    #endif // DEBUG

}


void CTRL_dealQueueMsg(void){
    if(msgQueueIsEmpty) return;
    CTRL_MSG tmp = msgQueueDequeue();
    dealMsg(&tmp);
}

