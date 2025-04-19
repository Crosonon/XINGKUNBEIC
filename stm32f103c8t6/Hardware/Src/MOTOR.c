#include "math.h"
#include "MOTOR.h"
#include "gpio.h"
#include "tim.h"

#define Motor_En_GPIO_Port GPIOB
#define Motor_Dir_GPIO_Port GPIOB
#define Motor_Step_GPIO_Port GPIOA

#define En 1
#define Dir 2
#define Step 3

uint8_t Tool = 0;
uint16_t Pin = 0x0000;

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
    if (Tool == Step)
    {
        if (MOTOR == 1 | MOTOR == 3)
        {
            Pin = Pin | Motor_1L_Step_Pin;
        }
        if (MOTOR == 2 | MOTOR == 3)
        {
            Pin = Pin | Motor_2H_Step_Pin;
        }
        return 3;
    }
    //setpin报错
    return 0;
}

void MOTOR_InitTool(void)
{
    Tool = 0;
    Pin = 0x0000;
}

/**
 * @brief 计算边缘误差，不需要给0即可
 * @param dis_cm 输入此时激光点与垂足的距离
**/
float MOTOR_K(float dis_cm)
{
    return pow(((pow(dis_cm, 2) + (10000) )/ 10000), 0.5);
}

void MOTOR_SetEn(uint8_t MOTOR, uint8_t MOTOR_En)
{
    Tool = En;
    if (MOTOR_SetPin(MOTOR))
    {
        HAL_GPIO_WritePin(Motor_En_GPIO_Port, Pin, MOTOR_En);
    }
    MOTOR_InitTool();
}

void MOTOR_SetDir(uint8_t MOTOR, uint8_t MOTOR_Dir)
{
    Tool = Dir;
    if (MOTOR_SetPin(MOTOR))
    {
        HAL_GPIO_WritePin(Motor_Dir_GPIO_Port, Pin, MOTOR_Dir);
    }
    MOTOR_InitTool();
}

void MOTOR_MoveStep(uint8_t MOTOR, uint8_t MOTOR_Step)
{
    MOTOR_Step = (MOTOR_Step < 8) ? MOTOR_Step : 8;
    Tool = Step;
    if (MOTOR_SetPin(MOTOR))
    {
        for (uint16_t i = 0; i < MOTOR_Step; i ++)
        {
            HAL_GPIO_WritePin(Motor_Step_GPIO_Port, Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(Motor_Step_GPIO_Port, Pin, GPIO_PIN_RESET);
            HAL_Delay(1);
        }
    }
    MOTOR_InitTool();
}

void MOTOR_MoveDist(uint8_t MOTOR, float MOTOR_Distance_cm)
{
    uint8_t MOTOR_Dir;
    if (MOTOR_Distance_cm > 0) MOTOR_Dir = 0;
    if (MOTOR_Distance_cm < 0) MOTOR_Dir = 1;
    MOTOR_SetDir(MOTOR, MOTOR_Dir);

    //步数 = 运动距离 / 步距，步距 = 间距1000mm * 步距角，步距角 = 3.14 / 100 / 细分
    //步数 = dis / (3.14 / 16) = dis / 0.196
    uint8_t MOTOR_Step = MOTOR_Distance_cm * MOTOR_K(0) / 0.196;
    MOTOR_MoveStep(MOTOR, MOTOR_Step);
}


void MOTOR_MoveLeft(float MOTOR_Distance_cm)
{
    MOTOR_MoveDist(1, MOTOR_Distance_cm);
}

void MOTOR_MoveRight(float MOTOR_Distance_cm)
{
    MOTOR_MoveLeft(-MOTOR_Distance_cm);
}

void MOTOR_MoveUp(float MOTOR_Distance_cm)
{
    MOTOR_MoveDist(2, MOTOR_Distance_cm);
}

void MOTOR_MoveDown(float MOTOR_Distance_cm)
{
    MOTOR_MoveUp(-MOTOR_Distance_cm);
}

/*曾经的pwm代码
// void MOTOR_SetFreq(int MOTOR, int Freq)
// {
//     Tool = Pwm;

//     //设定通道
//     if (MOTOR_SetChannel(MOTOR))
//     {
//         //0频率关闭
//         if (Freq == 0)
//         {
//             HAL_TIM_PWM_Stop(&htim1, Channel);
//             MOTOR_InitTool();
//             return;
//         }

//         //限幅1-100
//         Freq = (Freq < 1) ? 1 : (Freq > 100) ? 100 : Freq;

//         //arr计算
//         uint32_t arr = (1000000 / Freq) - 1;

//         //arr设置频率
//         __HAL_TIM_SET_AUTORELOAD(&htim1, arr);
//         __HAL_TIM_SET_COMPARE(&htim1, Channel, arr / 2);

//         //没开就打开
//         if (HAL_TIM_PWM_GetState(&htim1) != HAL_TIM_STATE_BUSY)
//         {
//             HAL_TIM_PWM_Start(&htim1, Channel);
//         }
//     }

//     //重置工具
//     MOTOR_InitTool();
// }
*/

/*曾经的通道设置代码
// uint8_t MOTOR_SetChannel(uint8_t MOTOR)
// {
//     if (Tool == Pwm)
//     {
//         if (MOTOR == 1)
//         {
//             Channel = TIM_CHANNEL_1;
//         }
//         else if (MOTOR == 2)
//         {
//             Channel = TIM_CHANNEL_2;
//         }
//         else if (MOTOR == 3)
//         {
//             Channel = TIM_CHANNEL_ALL;
//         }
//         return 1;
//     }
//     //setchannel报错
//     return 0;
// }
*/
