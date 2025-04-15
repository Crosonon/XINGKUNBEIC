#ifndef __MOTOR_H__
#define __MOTOR_H__

// void MOTOR_SetDuty(float duty);
// void MOTOR_SpeedDelta(float d);


void MOTOR_Init(void);
void MOTOR_SetEn(int MOTOR, int En);
void MOTOR_SetDir(int MOTOR, int Dir);
void MOTOR_SetFreq(int MOTOR, int Freq);

#endif
