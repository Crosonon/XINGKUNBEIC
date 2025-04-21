#include "Coordinate.h"
#include "math.h"

Pixel2D Pixel_Now = {0};
Pixel2D Pixel_Set = {0};
Pixel2D Pixel_Del = {0};
Distance2D Dis_Del = {0};

SystemParams sys_set = {
    .pixelx_to_cm = 0.1,
    .pixely_to_cm = 0.1,
    .origin_point = {50, 50},
    .Pixel_Corner_Set = {{0}}
};

void Coordinate_Init(void)
{
    sys_set.pixelx_to_cm = (
        (sys_set.Pixel_Corner_Set[0].x - sys_set.Pixel_Corner_Set[1].x) + 
        (sys_set.Pixel_Corner_Set[3].x - sys_set.Pixel_Corner_Set[2].x)
    ) / 2 / 50;

    sys_set.pixely_to_cm = (
        (sys_set.Pixel_Corner_Set[3].y - sys_set.Pixel_Corner_Set[0].y) + 
        (sys_set.Pixel_Corner_Set[2].y - sys_set.Pixel_Corner_Set[1].y)
    ) / 2 / 50;

    sys_set.origin_point.x = 0;
    sys_set.origin_point.y = 0;
    for (uint8_t i = 0; i < 4; i ++)
    {
        sys_set.origin_point.x += sys_set.Pixel_Corner_Set[i].x;
        sys_set.origin_point.y += sys_set.Pixel_Corner_Set[i].y;
    }
    sys_set.origin_point.x = sys_set.origin_point.x / 4;
    sys_set.origin_point.y = sys_set.origin_point.y / 4;
}

Pixel2D cm_to_Pixel(Distance2D dis2D)
{
    Pixel2D pix2D;
    pix2D.x = dis2D.x / sys_set.pixelx_to_cm;
    pix2D.y = dis2D.y / sys_set.pixely_to_cm;
    return pix2D;
}

Distance2D Pixel_to_cm(Pixel2D pix2D)
{
    Distance2D dis2D;
    dis2D.x = pix2D.x * sys_set.pixelx_to_cm;
    dis2D.y = pix2D.y * sys_set.pixely_to_cm;
    return dis2D;
}

float absDis(Distance2D dis2d)
{
    return powf((powf(dis2d.x, 2) + powf(dis2d.y, 2)), 0.5);
}
