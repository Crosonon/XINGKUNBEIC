
#ifndef __COORDINATE_H_
#define __COORDINATE_H_

#include "stm32f1xx_hal.h"
// #include "queue.h"

#define TARGET_QUEUE_SIZE 150  // 队列容量
#define TOTAL_STEP 5          //x和y移动总步数

typedef enum
{
    Mode_None,
    Mode_Origin,
    Mode_Square,
    Mode_A4Paper,
    Mode_Joystick,
}Control_Mode;

//二维像素点uint16_t
typedef struct 
{
    int32_t x;
    int32_t y;
} Pixel_Point;

//二维毫米点float
typedef struct 
{
    float x;
    float y;
} mm_Point;


typedef struct {
    Pixel_Point data[TARGET_QUEUE_SIZE];  // 存储目标点
    int front;                            // 队头索引
    int rear;                             // 队尾索引
    int size;                             // 当前元素数
} Point_Queue;

//标志位：初始化完毕否，读完本点否，读完所有点否，收到a4纸四个坐标否7
typedef struct
{
    uint8_t Init;//初始化后置1
    uint8_t Arrive;//读完本点置1
    uint8_t End;//结束置1
    uint8_t A4_Set;//得到了置1
}sys_flag;


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
    Point_Queue Cam_Point;
    //目标点队列
    Point_Queue Target_Point;
    //系统标志位
    sys_flag Flag;
    //屏幕距离
    float screen_dis;
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

typedef struct
{
    Pixel_Point start, target;
    mm_Point del;
    float k, b, theta, dis;
} Segment;

extern SystemParams sys_set;
extern Laser_Point_Ctrl laser;
extern Laser_Point_Watch laser_watch;

void Coordinate_Init(void);
Pixel_Point mm_to_Pixel(mm_Point mm_point);
mm_Point Pixel_to_mm(Pixel_Point pix_point);
float absDis(mm_Point dis2d);
uint8_t CheckUpdate_Del(void);
Pixel_Point Lerp_Pixel(Pixel_Point p1, Pixel_Point p2, float k);
void Segment_Init(Segment *seg, Pixel_Point start, Pixel_Point target);

#endif

