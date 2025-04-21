#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "stdint.h"
#include "gpio.h"
#include "Coordinate.h"

typedef enum
{
    Motor_Off = 0,
    Motor_On
} MOTOR_State;

typedef struct
{
    uint8_t subdivide;
    float Motor_I;
}MOTOR_Set;

extern MOTOR_Set motor_sys_set;

void MOTOR_Init(void);
void MOTOR_SetEn(uint8_t MOTOR, GPIO_PinState MOTOR_En);
uint8_t MOTOR_Dis_to_Step(float dis_cm);
float MOTOR_Step_to_Dis(uint8_t step);
void MOTOR_MoveLeft(float MOTOR_Distance_cm);
void MOTOR_MoveRight(float MOTOR_Distance_cm);
void MOTOR_MoveUp(float MOTOR_Distance_cm);
void MOTOR_MoveDown(float MOTOR_Distance_cm);
void MOTOR_MoveDis2D(Distance2D DisDel);
void MOTOR_Move(Distance2D* disdel);

// uint8_t MOTOR_SetPin(uint8_t MOTOR);
// void MOTOR_InitTool(void);
// void MOTOR_SetDir(uint8_t MOTOR, uint8_t MOTOR_Dir);
void MOTOR_MoveStep(uint8_t MOTOR, uint8_t MOTOR_Step);
// void MOTOR_MoveDist(uint8_t MOTOR, float MOTOR_Distance);

// void MOTOR_SetFreq(int MOTOR, int Freq);
// uint8_t MOTOR_SetChannel(uint8_t MOTOR);
// void MOTOR_SetDuty(float duty);
// void MOTOR_SpeedDelta(float d);

#endif
