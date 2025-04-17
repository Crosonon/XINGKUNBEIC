#include "stdint.h"
#include "gpio.h"
#include "KEY.h"

void KEY_Init(void);

uint8_t KEY_GetNum(void)
{
    uint8_t Key_Num = 0;

    if(HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin) == 0)
    {
        HAL_Delay(20);
        while(HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin) == 0);
        HAL_Delay(20);
        Key_Num = 1;
    }

    if(HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin) == 0)
    {
        HAL_Delay(20);
        while(HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin) == 0);
        HAL_Delay(20);
        Key_Num = 2;
    }

    return Key_Num;
}

void KEY_Act(uint8_t Key_Num)
{
    if (Key_Num == 1)
    {
        Key_Act1();
    }

    if (Key_Num == 2)
    {
        Key_Act2();
    }
}

//下面需要改动的
void Key_Act1(void)
{

}
void Key_Act2(void)
{

}