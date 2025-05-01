#include "usart.h"
#include "queue.h"
#include "Coordinate.h"
#include "Control.h"

Control_Mode Current_Mode = Mode_Joystick;
uint8_t a4_data[3] = {  0xff, 0x00, 0xfe };


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
        break;
    case Mode_Origin:
        Point_Queue_Enqueue(&(sys_set.Target_Point), sys_set.origin_point);
        sys_set.Flag.Arrive = 1;//这样中断中就会调取这个点
        break;
    case Mode_Square:
        for(int i = 3; i >= 0; i--)
        {
            Point_Queue_Enqueue(&(sys_set.Target_Point), sys_set.Calib_Point[i]);
            Point_Queue_Enqueue(&(sys_set.Target_Point), sys_set.Calib_Point[i]);
            for(int j = 1; j < 4; j++)
            {
                Point_Queue_Enqueue(&(sys_set.Target_Point), Lerp_Pixel(sys_set.Calib_Point[i],sys_set.Calib_Point[(i == 0) ? 3 : (i-1)],0.25 * j));
                Point_Queue_Enqueue(&(sys_set.Target_Point), Lerp_Pixel(sys_set.Calib_Point[i],sys_set.Calib_Point[(i == 0) ? 3 : (i-1)],0.25 * j));
            }
        }
        Point_Queue_Enqueue(&(sys_set.Target_Point), sys_set.Calib_Point[3]);
        Point_Queue_Enqueue(&(sys_set.Target_Point), sys_set.Calib_Point[3]);


        sys_set.Flag.Arrive = 1;
        break;
    case Mode_A4Paper:
        //发消息告诉k210要发消息给我
        //这里理应等一下，但是我有点不知道该怎么等
        //也许要先有点，再set这个模式

        a4_data[1] = 4;
        HAL_UART_Transmit(&huart2, a4_data, 3, 10000);
        HAL_Delay(1000);//第一次发送后 k210开始识别 需要等待的时间长一些
        for (uint8_t i = 0; i < 4; ++i)
        {
            a4_data[1] = i;
            HAL_UART_Transmit(&huart2, a4_data, 3, 10000);
            HAL_Delay(200);
        }
        HAL_Delay(500);

        if (sys_set.Flag.A4_Set == 0) //认为识别/传输失败
        {
            Pixel_Point point = Point_Queue_Dequeue(&(sys_set.Cam_Point));
            while (point.x < 1000 && point.x >= 0)
            {
                //清空Cam_Point队列数据
                point = Point_Queue_Dequeue(&(sys_set.Cam_Point));
            }
            return 0;
        }
        
        for(int i = 0; i < 4; i ++)
        {
            //一样检测是否0
            Pixel_Point point = Point_Queue_Dequeue(&(sys_set.Cam_Point));
            Point_Queue_Enqueue(&(sys_set.Target_Point), point);
        }
        
        sys_set.Flag.Arrive = 1;
        /* code */
        break;
    case Mode_Joystick:
        /* code */
        break;
    }
    sys_set.Flag.End = 0;
    return (uint8_t)Current_Mode;
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

