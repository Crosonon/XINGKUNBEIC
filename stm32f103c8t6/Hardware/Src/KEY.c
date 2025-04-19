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
    
    if(HAL_GPIO_ReadPin(Key3_GPIO_Port, Key3_Pin) == 0)
    {
        HAL_Delay(20);
        while(HAL_GPIO_ReadPin(Key3_GPIO_Port, Key3_Pin) == 0);
        HAL_Delay(20);
        Key_Num = 3;
    }
    
    if(HAL_GPIO_ReadPin(Key4_GPIO_Port, Key4_Pin) == 0)
    {
        HAL_Delay(20);
        while(HAL_GPIO_ReadPin(Key4_GPIO_Port, Key4_Pin) == 0);
        HAL_Delay(20);
        Key_Num = 4;
    }
    
    if(HAL_GPIO_ReadPin(Key5_GPIO_Port, Key5_Pin) == 0)
    {
        HAL_Delay(20);
        while(HAL_GPIO_ReadPin(Key5_GPIO_Port, Key5_Pin) == 0);
        HAL_Delay(20);
        Key_Num = 5;
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

    if (Key_Num == 3)
    {
        Key_Act3();
    }

    if (Key_Num == 4)
    {
        Key_Act4();
    }

    if (Key_Num == 5)
    {
        Key_Act5();
    }
}

//下面需要改动的
void Key_Act1(void)
{

}
void Key_Act2(void)
{

}
void Key_Act3(void)
{

}
void Key_Act4(void)
{

}
void Key_Act5(void)
{

}
