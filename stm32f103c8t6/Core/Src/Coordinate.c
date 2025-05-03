#include "Coordinate.h"
#include "math.h"

Laser_Point_Ctrl laser = {
    .Colour = "Red",
    .Now_Pix = {0, 0},
    .Set_Pix = {0, 0},
    .Del_mm = {0, 0},
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
    .Cam_Point = {
        .data = {65535,65535},
        .front = 0,
        .rear = -1,
        .size = 0,
    },
    .Target_Point = {
        .data = {65535,65535},
        .front = 0,
        .rear = -1,
        .size = 0,
    },
    .Flag = {
        .Init = 0,
        .Arrive = 1,
        .End = 1,
        .A4_Set = 0,
    }
};

void Coordinate_Init(void)
{
    //比例计算
    sys_set.x_pixel_to_mm = (float)500 / ((
        (sys_set.Calib_Point[0].x - sys_set.Calib_Point[1].x) + 
        (sys_set.Calib_Point[3].x - sys_set.Calib_Point[2].x)
    ) / 2);

    sys_set.y_pixel_to_mm = (float)500 / ((
        (sys_set.Calib_Point[3].y - sys_set.Calib_Point[0].y) + 
        (sys_set.Calib_Point[2].y - sys_set.Calib_Point[1].y)
    ) / 2);

    //原点设置
    mm_Point temp = {0,0};
    for (uint8_t i = 0; i < 4; i ++)
    {
        temp.x += sys_set.Calib_Point[i].x;
        temp.y += sys_set.Calib_Point[i].y;
    }
    sys_set.origin_point.x = (uint16_t)(temp.x / 4);
    sys_set.origin_point.y = (uint16_t)(temp.y / 4);

    sys_set.Flag.Init = 1;
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
    return fabs(powf((powf(dis2d.x, 2) + powf(dis2d.y, 2)), 0.5));
}


uint8_t CheckUpdate_Del(void)
{
    static Pixel_Point last_Set;
    static Pixel_Point last_Now;
    
    uint8_t Update = (last_Now.x != laser.Now_Pix.x || last_Now.y != laser.Now_Pix.y || last_Set.x != laser.Set_Pix.x || last_Set.y != laser.Set_Pix.y);
    if (Update)
    {
        Pixel_Point Del_Pix = {
            .x = laser.Set_Pix.x - laser.Now_Pix.x,
            .y = laser.Set_Pix.y - laser.Now_Pix.y
        };
        laser.Del_mm = Pixel_to_mm(Del_Pix);
        
        last_Set.x = laser.Set_Pix.x;
        last_Set.y = laser.Set_Pix.y;
        last_Now.x = laser.Now_Pix.x;
        last_Now.y = laser.Now_Pix.y;
    }
    return Update;
}

/**
    @brief 
    @param p1 第一个点
    @param p2 第二个点
    @param k  比例系数，在0-1之间，越小则距离点p1更近，0则返回p1
*/
Pixel_Point Lerp_Pixel(Pixel_Point p1, Pixel_Point p2, float k)
{
    Pixel_Point p;
    p.x = p1.x * (1 - k) + p2.x * k;
    p.y = p1.y * (1 - k) + p2.y * k;
    return p;
}

/**
 * @brief 初始化线段
 */
void Segment_Init(Segment *seg, Pixel_Point start, Pixel_Point target)
{
    seg->start = start;
    seg->target = target;
    seg->k = (float)(target.y - start.y) / (target.x - start.x);
    seg->b = start.y - seg->k * start.x;
    seg->dis = sqrt(pow((target.x - start.x), 2) + pow((target.y - start.y), 2));
    seg->theta = atan(seg->k);
    Pixel_Point tmp = {
        .x = target.x - start.x,
        .y = target.y - start.y
    };
    seg->del = Pixel_to_mm(tmp);
}
