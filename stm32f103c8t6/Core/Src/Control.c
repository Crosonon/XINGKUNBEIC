#include "Control.h"
#include "queue.h"
#include "Coordinate.h"

Control_Mode Current_Mode = Mode_None;



/*函数：
设置模式：输入要设置的模式，返回设置成功或者失败
里面写入队列
*/
void Control_Init(void)
{
    Point_Queue_Init(&(sys_set.Target_Point));
}

uint8_t Control_SetMode(Control_Mode Set_Mode)
{
    
    Current_Mode = Set_Mode;
    Point_Queue_Init(&(sys_set.Target_Point));

    if(sys_set.Flag.Init == 0) return 0;//还没初始化呢哥

    switch (Current_Mode)
    {
    case Mode_None:
        return Mode_None;
        break;
    case Mode_Origin:
        //如果原点不为0
        Point_Queue_Enqueue(&(sys_set.Target_Point), sys_set.origin_point);
        break;
    case Mode_Square:
        for(int i = 0; i < 4; i ++)
        {
            //一样检测是否0
            Point_Queue_Enqueue(&(sys_set.Target_Point), sys_set.Calib_Point[i]);
        }
        break;
    case Mode_A4Paper:
        //发消息告诉k210要发消息给我
        //这里理应等一下，但是我有点不知道该怎么等
        //也许要先有点，再set这个模式
        if (sys_set.Flag.A4_Set == 0) return 0;
        
        for(int i = 0; i < 4; i ++)
        {
            //一样检测是否0
            Pixel_Point point = Point_Queue_Dequeue(&(sys_set.Cam_Point));
            Point_Queue_Enqueue(&(sys_set.Target_Point), point);
        }
        /* code */
        break;
    case Mode_Joystick:
        /* code */
        break;
    // case Mode_Square:
    //     /* code */
    //     break;
    // case Mode_A4Paper:
    //     /* code */
    //     break;
    }
    sys_set.Flag.End = 0;
    return Current_Mode;
}

uint8_t Control_EndMode(void)
{
    switch (Current_Mode)
    {
    case Mode_None:
        /* code */
        break;
    case Mode_Origin:
        //原点模式结束
        break;
    case Mode_Square:
        //方形模式结束
        break;
    case Mode_A4Paper:
        //a4模式结束
        sys_set.Flag.A4_Set = 0;
        break;
    case Mode_Joystick:
        /* code */
        break;
    }
    Current_Mode = Mode_None;
    return Current_Mode;
}

