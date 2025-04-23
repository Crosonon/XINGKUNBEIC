#include "Coordinate.h"
#include "math.h"

Laser_Point_Ctrl laser = {
    .Colour = "Red",
    .Now_Pix = {0, 0},
    .Set_Pix = {0, 0},
    .Del_mm = {0, 0},
    .Arrive_flag = 0,
    .correct = {
        .State = DISABLE,
        .h = 1000,
        .Pedal_Pix = {0,0},
        .Correct_K = 1,
    }
};

Laser_Point_Watch laser_watch = {
    .Colour = "Red",
    .Now_Pix = {0, 0},
    .Correct = DISABLE,
    .Correct_K = 1,
};


SystemParams sys_set = {
    .x_pixel_to_mm = 0.1,
    .y_pixel_to_mm = 0.1,
    .origin_point = {0, 0},
    .Calib_Point = {{0,0}},
    .Cam_Point = {0,0},
};

void Coordinate_Init(void)
{
    //比例计算
    sys_set.x_pixel_to_mm = (
        (sys_set.Calib_Point[0].x - sys_set.Calib_Point[1].x) + 
        (sys_set.Calib_Point[3].x - sys_set.Calib_Point[2].x)
    ) / 2 / 500;

    sys_set.y_pixel_to_mm = (
        (sys_set.Calib_Point[3].y - sys_set.Calib_Point[0].y) + 
        (sys_set.Calib_Point[2].y - sys_set.Calib_Point[1].y)
    ) / 2 / 500;

    //原点设置
    mm_Point temp = {0,0};
    for (uint8_t i = 0; i < 4; i ++)
    {
        temp.x += sys_set.Calib_Point[i].x;
        temp.y += sys_set.Calib_Point[i].y;
    }
    sys_set.origin_point.x = (uint16_t)(temp.x / 4);
    sys_set.origin_point.y = (uint16_t)(temp.y / 4);
}

Pixel_Point mm_to_Pixel(mm_Point mm_point)
{
    Pixel_Point temp_Pix;
    temp_Pix.x = mm_point.x / sys_set.x_pixel_to_mm;
    temp_Pix.y = mm_point.y / sys_set.y_pixel_to_mm;
    return temp_Pix;
}

mm_Point Pixel_to_mm(Pixel_Point pix_point)
{
    mm_Point temp_mm;
    temp_mm.x = pix_point.x * sys_set.x_pixel_to_mm;
    temp_mm.y = pix_point.y * sys_set.y_pixel_to_mm;
    return temp_mm;
}

float absDis(mm_Point dis2d)
{
    return powf((powf(dis2d.x, 2) + powf(dis2d.y, 2)), 0.5);
}


uint8_t CheckUpdate_Del(void)
{
    static Pixel_Point last_Set;
    static Pixel_Point last_Now;
    uint8_t Update = ((last_Now.x - laser.Now_Pix.x) | (last_Now.y - laser.Now_Pix.y)) | ((last_Set.x - laser.Set_Pix.x) | (last_Set.y - laser.Set_Pix.y));
    if (Update)
    {
        Pixel_Point Del_Pix = {
            Del_Pix.x = laser.Set_Pix.x - laser.Now_Pix.x,
            Del_Pix.y = laser.Set_Pix.y - laser.Now_Pix.y,
            };
        laser.Del_mm = Pixel_to_mm(Del_Pix);
        
        last_Set = laser.Set_Pix;
        last_Now = laser.Now_Pix;
    }
    return Update;
}
