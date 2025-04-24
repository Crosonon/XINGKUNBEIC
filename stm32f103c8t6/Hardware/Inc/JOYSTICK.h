#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

extern float JoyxCH0;
extern float JoyyCH1;

#include "Motor.h"

Motor_Dir_State Joy_Get_X(void);
Motor_Dir_State Joy_Get_Y(void);

#endif
