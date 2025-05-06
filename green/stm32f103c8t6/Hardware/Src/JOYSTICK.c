#include "JOYSTICK.h"
#include "MOTOR.h"

float JoyxCH0;
float JoyyCH1;

//摇杆控制：使set为now＋1

//获取x的方向
Motor_Dir_State Joy_Get_X(void)
{
    if(JoyxCH0 < 0.5) return Right;
    else if(JoyxCH0 > 3) return Left;
    else return Stop;
}
//获取y的方向
Motor_Dir_State Joy_Get_Y(void)
{
    if(JoyyCH1 < 0.5) return Down;
    else if(JoyyCH1 > 3) return Up;
    else return Stop;
}

