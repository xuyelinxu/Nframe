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
#include "stm32f10x_gpio.h"
#include "nframe.h"

#define myGPIO  GPIOE
#define myPIN   GPIO_Pin_2
/**
* \brief 测试子程序
*/
void test_func(void)
{
    static BOOLEAN ledOn = FALSE;   /* 布尔静态变量 表示LED */

    if(ledOn){
        GPIO_SetBits(myGPIO, myPIN);
    }
    else{
        GPIO_ResetBits(myGPIO, myPIN);
    }

    ledOn = !ledOn;     /* 翻转状态 */
}

void gpio_init(void)
{
    GPIO_InitTypeDef gpioInitStruct;
    gpioInitStruct.GPIO_Mode    = GPIO_Mode_Out_PP;
    gpioInitStruct.GPIO_Pin     = myPIN;
    gpioInitStruct.GPIO_Speed   = GPIO_Speed_2MHz;
    GPIO_Init(myGPIO, &gpioInitStruct);
}

/**
* \brief 主函数中调用NFTASK模块
*/
int main(void)
{
    NFTASK_SetupTypeDef taskSetup;

    NFRAME_Init();      /* NFrame初始化 */
    gpio_init();        /* GPIO初始化 */

    taskSetup.Function          = test_func;
    taskSetup.NFTASK_Time_Unit  = NFTASK_TIME_UNIT_MS;
    taskSetup.Time              = 500;
    NFTASK_Setup(&taskSetup, NFTASK_TYPE_ENDLESS);

    while(1)
    {
        NFRAME_Run();
    }
}

void assert_failed(uint8_t* file, uint32_t line)
{
    while (1);
}
