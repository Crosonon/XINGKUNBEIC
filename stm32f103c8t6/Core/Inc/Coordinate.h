
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
    float x_pixel_to_mm;
    float y_pixel_to_mm;
    Pixel_Point origin_point;
    Pixel_Point Calib_Point[4];
    Pixel_Point Cam_Point;
} SystemParams;

typedef struct 
{
    char Colour[5];
    Pixel_Point Now_Pix;
    Pixel_Point Set_Pix;
    mm_Point Del_mm;

    //矫正
    FunctionalState Correct;
    Pixel_Point Pedal_Pix;
    float Correct_K;
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


#endif

