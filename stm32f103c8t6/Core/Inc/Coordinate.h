#ifndef __COORDINATE_H
#define __COORDINATE_H

#include "stm32f1xx_hal.h"

typedef struct 
{
    uint16_t x;
    uint16_t y;
} Pixel2D;

typedef struct 
{
    float x;
    float y;
} Distance2D;

typedef struct 
{
    float pixelx_to_cm;
    float pixely_to_cm;
    Pixel2D origin_point;
    Pixel2D Pixel_Corner_Set[4];
} SystemParams;

extern Pixel2D Pixel_Now;
extern Pixel2D Pixel_Set;
extern Pixel2D Pixel_Del;
extern Distance2D Dis_Del;
extern SystemParams sys_set;

void Coordinate_Init(void);
Pixel2D cm_to_Pixel(Distance2D dis2D);
Distance2D Pixel_to_cm(Pixel2D pix2D);
float absDis(Distance2D dis2d);


#endif

