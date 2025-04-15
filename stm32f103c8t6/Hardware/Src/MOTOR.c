#include "MOTOR.h"
#include "tim.h"

void MOTOR_Init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1 | TIM_CHANNEL_2);
} 
void MOTOR_SetEn(int MOTOR, int En)
{
    if (MOTOR == 1 | MOTOR == 3)
    {
        HAL_GPIO_WritePin(Motor_1L_En_GPIO_Port,Motor_1L_En_Pin,En);
    }
    if (MOTOR == 2 | MOTOR == 3)
    {
        HAL_GPIO_WritePin(Motor_2H_En_GPIO_Port,Motor_2H_En_Pin,En);
    }
}

void MOTOR_SetDir(int MOTOR, int Dir)
{
    if (MOTOR == 1 | MOTOR == 3)
    {
        HAL_GPIO_WritePin(Motor_1L_En_GPIO_Port,Motor_1L_En_Pin,Dir);
    }
    if (MOTOR == 2 | MOTOR == 3)
    {
        HAL_GPIO_WritePin(Motor_2H_En_GPIO_Port,Motor_2H_En_Pin,Dir);
    }
}

void MOTOR_SetFreq(int MOTOR, int Freq)
{

}