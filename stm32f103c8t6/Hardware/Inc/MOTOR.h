#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "stdint.h"
#include "gpio.h"
#include "Coordinate.h"

typedef struct 
{
    GPIO_TypeDef* En_Port;
    uint16_t En_Pin;
    GPIO_TypeDef* Dir_Port;
    uint16_t Dir_Pin;
    GPIO_TypeDef* Step_Port;
    uint16_t Step_Pin;
} Motor_Pin_Config;


//使能状态，由物理开关控制

//锁定状态（锁定为不能动，解锁为能动）,在有需要的地方设置
typedef enum
{
    Unlocked = 0u,
    Locked = 1u,
} Motor_Lock_State;

//方向状态，在驱动中断中通过判断disdel设置
typedef enum
{
    Stop = 0,
    Up,
    Right,
    Down,
    Left,
} Motor_Dir_State;

//总状态：序号使能锁定方向速度
typedef struct 
{
    uint8_t Number;
    FunctionalState En;
    Motor_Lock_State Lock;
    Motor_Dir_State Dir;
    uint8_t Speed;
    Motor_Pin_Config Pin_Config;
} Motor_Config;

//驱动模块设置，细分和电流
typedef struct
{
    uint8_t Subdivide;
    float Current;
    Motor_Lock_State Lock;
} Motor_Drive_Set;

extern Motor_Drive_Set motor_drive_set;
extern Motor_Config motor_1L ;
extern Motor_Config motor_2H ;

void Motor_Init(void);
uint8_t Motor_Move_Unit(Motor_Config motor);
uint8_t Motor_Move_MutiUnit(Motor_Config motor, uint16_t times);
uint8_t Motor_Move_Step(Motor_Config motor);

uint8_t Motor_Update_Position(Motor_Config* motor, float* del, float step_dis);

uint8_t Motor_Dir_Set(Motor_Config* motor1, Motor_Config* motor2, mm_Point diedel);
uint8_t Motor_Lock_Check(void);
float Motor_Step_Dis(Motor_Config motor, Laser_Point_Ctrl laser);


#endif
