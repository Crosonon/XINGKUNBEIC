
#ifndef __COORDINATE_H_
#define __COORDINATE_H_

#include "stm32f1xx_hal.h"

//二维像素点uint16_t
typedef struct 
{
    uint16_t x;
    uint16_t y;
} Pixel_Point;

//二维毫米点float
typedef struct 
{
    float x;
    float y;
} mm_Point;

typedef struct 
{
    //换算系数
    float x_pixel_to_mm;
    float y_pixel_to_mm;
    //原点坐标
    Pixel_Point origin_point;
    //校准点坐标
    Pixel_Point Calib_Point[4];
    //摄像头传来的点
    Pixel_Point Cam_Point;

} SystemParams;

typedef struct
{
    //开关
    FunctionalState State;
    //垂直距离
    float h;
    //垂足坐标
    Pixel_Point Pedal_Pix;
    //比例系数（大于等于1）
    float Correct_K;
} Correct;


typedef struct 
{
    //颜色
    char Colour[5];
    //看到的位置
    Pixel_Point Now_Pix;
    //要去的位置
    Pixel_Point Set_Pix;
    //set-now，向右下为正，单位毫米
    mm_Point Del_mm;
    //到达标志位，1为到，0为不到
    uint8_t Arrive_flag;
    //校准情况
    Correct correct;
} Laser_Point_Ctrl;


typedef struct 
{
    char Colour[5];
    Pixel_Point Now_Pix;

    //矫正
    FunctionalState Correct;
    Pixel_Point Pedal_Pix;
    float Correct_K;
} Laser_Point_Watch;

extern SystemParams sys_set;
extern Laser_Point_Ctrl laser;
extern Laser_Point_Watch laser_watch;

void Coordinate_Init(void);
Pixel_Point mm_to_Pixel(mm_Point mm_point);
mm_Point Pixel_to_mm(Pixel_Point pix_point);
float absDis(mm_Point dis2d);
uint8_t CheckUpdate_Del(void);


#endif

