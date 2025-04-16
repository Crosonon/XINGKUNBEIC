#ifndef __MOTOR_H__
#define __MOTOR_H__

// void MOTOR_SetDuty(float duty);
// void MOTOR_SpeedDelta(float d);


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

void MOTOR_Init(void);
uint8_t MOTOR_SetPin(uint8_t MOTOR);
uint8_t MOTOR_SetChannel(uint8_t MOTOR);
void MOTOR_InitTool(void);
void MOTOR_SetEn(int MOTOR, int MOTOR_En);
void MOTOR_SetDir(int MOTOR, int MOTOR_Dir);
void MOTOR_SetFreq(int MOTOR, int Freq);


#endif
