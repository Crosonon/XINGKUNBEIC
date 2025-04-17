#include "MOTOR.h"
#include "tim.h"

#define Motor_En_GPIO_Port GPIOB
#define Motor_Dir_GPIO_Port GPIOB
#define Motor_Pwm_GPIO_Port GPIOA

#define En 1
#define Dir 2
#define Pwm 3

uint8_t Tool = 0;
uint16_t Pin = 0x0000;
uint16_t Channel = 0x00000000U;

void MOTOR_Init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1 | TIM_CHANNEL_2);
} 

uint8_t MOTOR_SetPin(uint8_t MOTOR)
{
    if (Tool == En)
    {
        if (MOTOR == 1 | MOTOR == 3)
        {
            Pin = Pin | Motor_1L_En_Pin;
        }
        if (MOTOR == 2 | MOTOR == 3)
        {
            Pin = Pin | Motor_2H_En_Pin;
        }
        return 1;
    }
    if (Tool == Dir)
    {
        if (MOTOR == 1 | MOTOR == 3)
        {
            Pin = Pin | Motor_1L_Dir_Pin;
        }
        if (MOTOR == 2 | MOTOR == 3)
        {
            Pin = Pin | Motor_2H_Dir_Pin;
        }
        return 2;
    }
    if (Tool == Pwm)
    {
        if (MOTOR == 1 | MOTOR == 3)
        {
            Pin = Pin | Motor1_PWM_Pin;
        }
        if (MOTOR == 2 | MOTOR == 3)
        {
            Pin = Pin | Motor2_PWM_Pin;
        }
        return 3;
    }
    //setpin报错
    return 0;
}

uint8_t MOTOR_SetChannel(uint8_t MOTOR)
{
    if (Tool == Pwm)
    {
        if (MOTOR == 1)
        {
            Channel = TIM_CHANNEL_1;
        }
        else if (MOTOR == 2)
        {
            Channel = TIM_CHANNEL_2;
        }
        else if (MOTOR == 3)
        {
            Channel = TIM_CHANNEL_ALL;
        }
        return 1;
    }
    //setchannel报错
    return 0;
}

void MOTOR_InitTool(void)
{
    Tool = 0;
    Pin = 0x0000;
    Channel = 0x00000000U;
}

void MOTOR_SetEn(int MOTOR, int MOTOR_En)
{
    Tool = En;
    if (MOTOR_SetPin(MOTOR))
    {
        HAL_GPIO_WritePin(Motor_En_GPIO_Port, Pin, MOTOR_En);
    }
    MOTOR_InitTool();
}

void MOTOR_SetDir(int MOTOR, int MOTOR_Dir)
{
    Tool = Dir;
    if (MOTOR_SetPin(MOTOR))
    {
        HAL_GPIO_WritePin(Motor_Dir_GPIO_Port, Pin, MOTOR_Dir);
    }
    MOTOR_InitTool();
}

//由于单个tim不能设置多个频率，这里两个电机实际上一致
void MOTOR_SetFreq(int MOTOR, int Freq)
{
    Tool = Pwm;

    //设定通道
    if (MOTOR_SetChannel(MOTOR))
    {
        //0频率关闭
        if (Freq == 0)
        {
            HAL_TIM_PWM_Stop(&htim1, Channel);
            MOTOR_InitTool();
            return;
        }

        //限幅1-100
        Freq = (Freq < 1) ? 1 : (Freq > 100) ? 100 : Freq;

        //arr计算
        uint32_t arr = (1000000 / Freq) - 1;

        //arr设置频率
        __HAL_TIM_SET_AUTORELOAD(&htim1, arr);
        __HAL_TIM_SET_COMPARE(&htim1, Channel, arr / 2);

        //没开就打开
        if (HAL_TIM_PWM_GetState(&htim1) != HAL_TIM_STATE_BUSY)
        {
            HAL_TIM_PWM_Start(&htim1, Channel);
        }
    }

    //重置工具
    MOTOR_InitTool();
}

void MOTOR_Move(int MOTOR, int Step)
{
    if (MOTOR == 1)
    {

    }
}
