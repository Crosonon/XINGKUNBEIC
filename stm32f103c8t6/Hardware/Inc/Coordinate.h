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
    float pixel_to_cm;
    Pixel2D origin_point;
} SystemParams;

extern Pixel2D Pixel_Now;
extern Pixel2D Pixel_Set;
extern Pixel2D Pixel_Del;
extern Pixel2D Pixel_Corner_Set;



#endif

