#ifndef __MOTOR_H__
#define __MOTOR_H__


#include "stdint.h"

void MOTOR_Init(void);
uint8_t MOTOR_SetPin(uint8_t MOTOR);
void MOTOR_InitTool(void);
void MOTOR_SetEn(uint8_t MOTOR, uint8_t MOTOR_En);
void MOTOR_SetDir(uint8_t MOTOR, uint8_t MOTOR_Dir);
void MOTOR_MoveStep(uint8_t MOTOR, uint8_t MOTOR_Step);
void MOTOR_MoveDist(uint8_t MOTOR, float MOTOR_Distance);

// void MOTOR_SetFreq(int MOTOR, int Freq);
// uint8_t MOTOR_SetChannel(uint8_t MOTOR);
// void MOTOR_SetDuty(float duty);
// void MOTOR_SpeedDelta(float d);

#endif
