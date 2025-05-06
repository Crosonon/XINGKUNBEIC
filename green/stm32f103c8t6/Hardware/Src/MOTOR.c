#include <math.h>
#include "MOTOR.h"
#include "gpio.h"
#include "Coordinate.h"
#include "OLED.h"

#define abs(x) (x > 0 ? x : (-x))

//电机驱动模块设置，全局变量，需要手动更改
Motor_Drive_Set motor_drive_set = {
    .Subdivide = 32,
    .Current = 1.44,
    .Lock = Unlocked,
    //添加步距
};
//下电机使能锁定方向速度
Motor_Config motor_1L = {
    .Number = 1,
    .En = ENABLE,
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
    .En = ENABLE,
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

void Motor_Init(void)
{
    motor_1L.En = ENABLE;
    motor_2H.En = ENABLE;
    HAL_GPIO_WritePin(motor_1L.Pin_Config.En_Port, motor_1L.Pin_Config.En_Pin, (GPIO_PinState)motor_1L.En);
    HAL_GPIO_WritePin(motor_2H.Pin_Config.En_Port, motor_2H.Pin_Config.En_Pin, (GPIO_PinState)motor_2H.En);
} 

/*电机移动最小步距unit，移动一步step*/
uint8_t Motor_Move_Unit(Motor_Config motor)
{
    //如果不使能、锁定模式、不动，就不走一步
    if((motor.En == DISABLE) || (motor.Lock == Locked) || (motor.Dir == Stop)) return 0;

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
/**
    @brief 唯一外部调用运动：更新位置，会把电机动一步，然后修正del的值
    @param motor 使用的电机
    @param del 
    @param step_dis 步距，用于修正del
*/
uint8_t Motor_Update_Position(Motor_Config* motor, float* del, float step_dis)
{
    // if(Motor_Move_MutiUnit(*motor, motor->Speed))
    if(Motor_Move_Unit(*motor))
    {
        switch (motor->Dir)
        {
            case Right:
            case Down:
                *del -= step_dis;
                break;

            case Left:
            case Up:
                *del += step_dis;
                break;

            default:
                return 0;
        }
        return 1;
    }
    return 0;
}

//根据del的大小设定电机方向

uint8_t Motor_Dir_Set(Motor_Config* motor1, Motor_Config* motor2, mm_Point diedel)
{
    if(fabs(diedel.x) <= 8 && fabs(diedel.y) <= 8)//如果距离小
    {
        //到达！
        motor1->Dir = Stop;
        motor2->Dir = Stop;

        //
        // HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
        //
        return 0;
    }
    HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);

    motor1->Dir = (diedel.x > 5) ? Right : (diedel.x < -5) ? Left : Stop;
    motor2->Dir = (diedel.y > 5) ? Down : (diedel.y < -5) ? Up : Stop;
    return 1;
}

//会根据switch的输入更新两个电机的锁定
//硬件问题已禁用
uint8_t Motor_Lock_Check(void)
{
    motor_drive_set.Lock = (Motor_Lock_State)HAL_GPIO_ReadPin(Motor_Switch_GPIO_Port, Motor_Switch_Pin);

    motor_1L.Lock = motor_drive_set.Lock;
    motor_2H.Lock = motor_drive_set.Lock;

    return (uint8_t)motor_drive_set.Lock;
}

//大于等于1
float Laser_Correct(Laser_Point_Ctrl laser)
{
    if (laser.correct.State == DISABLE) return 1;
    return 1;
    // laser.correct.Pedal_Pix
    /*
    在减小disdel的时候调用
    如果打开，就算k然后乘上去
    怎么算k？使用循环调用
    先通过系统比例算出点的点距（now-垂足），*系统比例\*积分到k得到大概距离（float），由此算出k
    多用几次是否能矫正？这个要算一下
    */
    // return pow(((pow(d,2)+(1000000))/1000000),0.5);
}

//移动一step的长度（矫正后）
float Motor_Step_Dis(Motor_Config motor, Laser_Point_Ctrl laser)
{
    //步距=垂直距离*角度*矫正系数，角度=速度*3.14/100 /细分
    // float theta = motor.Speed * 3.14 / 100 / motor_drive_set.Subdivide;
    // return laser.correct.h * theta * Laser_Correct(laser);


    //假设摄像头在白板中心，理论上计算结果是精确的，但由于云台和摄像头不在同一个点，所以存在误差
    //计算过程注释写不清楚
    float theta_x = motor.Speed * 3.1415926535 / 100 / motor_drive_set.Subdivide,
          theta_y = theta_x;
    Pixel_Point del = {
        .x = laser.Now_Pix.x - sys_set.origin_point.x,
        .y = laser.Now_Pix.y - sys_set.origin_point.y
    };
    mm_Point del_mm = Pixel_to_mm(del);
    float screen_dis_corr_x = sqrt(pow(sys_set.screen_dis, 2) + pow(del_mm.x, 2)),
          screen_dis_corr_y = sqrt(pow(sys_set.screen_dis, 2) + pow(del_mm.y, 2));
    float alpha_x = atan(del_mm.x / screen_dis_corr_y),
          alpha_y = atan(del_mm.y / screen_dis_corr_x);
    if (motor.Dir == Left) theta_x = -theta_x;
    if (motor.Dir == Down) theta_y = -theta_y;
    float step_dis_x = fabs(tan(alpha_x + theta_x) * screen_dis_corr_y - del_mm.x),
          step_dis_y = fabs(tan(alpha_y + theta_x) * screen_dis_corr_x - del_mm.y);
    if (motor.Number == 1)
        return step_dis_x;
    else
        return step_dis_y;
}

