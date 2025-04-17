#ifndef __MOTOR_H__
#define __MOTOR_H__


#include "stdint.h"

void MOTOR_Init(void);
uint8_t MOTOR_SetPin(uint8_t MOTOR);
uint8_t MOTOR_SetChannel(uint8_t MOTOR);
void MOTOR_InitTool(void);
void MOTOR_SetEn(int MOTOR, int MOTOR_En);
void MOTOR_SetDir(int MOTOR, int MOTOR_Dir);
void MOTOR_SetFreq(int MOTOR, int Freq);

// void MOTOR_SetDuty(float duty);
// void MOTOR_SpeedDelta(float d);

#endif
