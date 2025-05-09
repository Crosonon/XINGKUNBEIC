#include "BEEP.h"

//申请beep，可在任何地方调用，单位秒，一般给0.5
void Beep_Request(float t)
{
    Beep_time = time + t * 100;
}

//在定时中断中调用，用于更新beep；
void Beep_Update(void)
{
    //没设置直接返回
    if(Beep_time == 0)
    {
        return;
    }
    //还有时间但是没开就打开
    else if ((Beep_time > time) && (HAL_GPIO_ReadPin(Beep_GPIO_Port, Beep_Pin) == 1))
    {
        HAL_GPIO_WritePin(Beep_GPIO_Port, Beep_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
        return;
    }
    //没时间了就关上
    else if (Beep_time <= time)
    {
        HAL_GPIO_WritePin(Beep_GPIO_Port, Beep_Pin, GPIO_PIN_SET); 
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
        Beep_time = 0;
        return;
    }
}

