#include "math.h"
#include "MOTOR.h"
#include "gpio.h"
#include "Coordinate.h"

//电机驱动模块设置，全局变量，需要手动更改
Motor_Drive_Set motor_drive_set = {
    .Subdivide = 32,
    .Current = 1.44
};
//下电机使能锁定方向速度
Motor_Config motor_1L = {
    .Number = 1,
    .En = Enable,
    .Lock = Unlocked,
    .Dir = Stop,
    .Speed = 1,
    .Pin_Config = {
        .En_Port = Motor_1L_En_GPIO_Port,
        .En_Pin = Motor_1L_En_Pin,
        .Dir_Port = Motor_1L_Dir_GPIO_Port,
        .Dir_Pin = Motor_1L_Dir_Pin,
        .Step_Port = Motor_1L_Step_GPIO_Port,
        .Step_Pin = Motor_1L_Step_Pin,
    }
};
//上电机使能锁定方向速度
Motor_Config motor_2H = {
    .Number = 2,
    .En = Enable,
    .Lock = Unlocked,
    .Dir = Stop,
    .Speed = 1,
    .Pin_Config = {
        .En_Port = Motor_2H_En_GPIO_Port,
        .En_Pin = Motor_2H_En_Pin,
        .Dir_Port = Motor_2H_Dir_GPIO_Port,
        .Dir_Pin = Motor_2H_Dir_Pin,
        .Step_Port = Motor_2H_Step_GPIO_Port,
        .Step_Pin = Motor_2H_Step_Pin,
    }
};

void MOTOR_Init(void)
{
    
} 

/*新函数：电机移动最小步距unit，移动一步step*/

uint8_t Motor_Move_Unit(Motor_Config motor)
{
    //如果不使能、锁定模式、不动，就不走一步
    if((motor.En == Disable) || (motor.Lock == Locked) || (motor.Dir == Stop)) return 0;

    //设置方向
    if(motor.Dir == Up || motor.Dir == Right) HAL_GPIO_WritePin(motor.Pin_Config.Dir_Port, motor.Pin_Config.Dir_Pin, GPIO_PIN_RESET);
    else if(motor.Dir == Down || motor.Dir == Left) HAL_GPIO_WritePin(motor.Pin_Config.Dir_Port, motor.Pin_Config.Dir_Pin, GPIO_PIN_SET);

    //给个脉冲
    HAL_GPIO_WritePin(motor.Pin_Config.Step_Port, motor.Pin_Config.Step_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(motor.Pin_Config.Step_Port, motor.Pin_Config.Step_Pin, GPIO_PIN_SET);

    return 1;
}

//移动多个单位
uint8_t Motor_Move_MutiUnit(Motor_Config motor, uint16_t times)
{
    uint8_t temp = 0;
    for(uint8_t i = times; i > 0; i --)
    {
        temp = Motor_Move_Unit(motor);
    }
    return temp;
}

//速度将有影响
uint8_t Motor_Move_Step(Motor_Config motor)
{
    return Motor_Move_MutiUnit(motor, motor.Speed);
}
