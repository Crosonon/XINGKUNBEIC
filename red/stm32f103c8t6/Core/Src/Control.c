#include "usart.h"
#include "queue.h"
#include "Coordinate.h"
#include "Control.h"
#include "tim.h"

Control_Mode Current_Mode = Mode_Joystick;
uint8_t a4_data[3] = {  0xff, 0x00, 0xfe };
Point_Queue Track_Point;

float Speed_k = 1; 

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
        HAL_TIM_Base_Stop_IT(&htim3);
        htim3.Init.Period = 10000/Speed_k-1;
        if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
        {
            Error_Handler();
        }

        Point_Queue_Enqueue(&(sys_set.Target_Point), sys_set.origin_point);
        Point_Queue_Enqueue(&(sys_set.Target_Point), sys_set.origin_point);
        Point_Queue_Enqueue(&(sys_set.Target_Point), sys_set.origin_point);
        Point_Queue_Enqueue(&(sys_set.Target_Point), sys_set.origin_point);

        sys_set.Flag.Arrive = 1;//这样中断中就会调取这个点
        HAL_TIM_Base_Start_IT(&htim3);
        break;
    case Mode_Square:
        HAL_TIM_Base_Stop_IT(&htim3);
        htim3.Init.Period = 10000/Speed_k-1;
        if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
        {
            Error_Handler();
        }

        for(int i = 4; i >= 0; i--)
        {
            Point_Queue_Enqueue(&(sys_set.Target_Point), sys_set.Calib_Point[i % 4]);
        }

        Point_Queue_Lerp(&(sys_set.Target_Point), 4);
        Point_Queue_Double(&(sys_set.Target_Point));

        sys_set.Flag.Arrive = 1;
        HAL_TIM_Base_Start_IT(&htim3);
        break;

    case Mode_A4Paper:
        HAL_TIM_Base_Stop_IT(&htim3);
        htim3.Init.Period = 20000/Speed_k-1;
        if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
        {
            Error_Handler();
        }

        a4_data[1] = 4;
        HAL_UART_Transmit(&huart2, a4_data, 3, 10000);
        HAL_Delay(1000);//第一次发送后 k210开始识别 需要等待的时间长一些
        for (uint8_t i = 0; i < 4; ++i)
        {
            a4_data[1] = i;
            HAL_UART_Transmit(&huart2, a4_data, 3, 10000);
            HAL_Delay(200);
        }
        HAL_Delay(1000);

        if (sys_set.Cam_Point.size < 4) //认为识别/传输失败
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
        if (sys_set.Target_Point.size != 4) return 0;

        Point_Queue_Enqueue(&(sys_set.Target_Point), Point_Queue_Peek(&(sys_set.Target_Point)));
        Point_Queue_Lerp(&(sys_set.Target_Point), 2);
        Point_Queue_Double(&(sys_set.Target_Point));

        // sys_set.Flag.A4_Set = 0;
        sys_set.Flag.Arrive = 1;
        HAL_TIM_Base_Start_IT(&htim3);
        /* code */
        break;
    case Mode_Joystick:
        /* code */
        break;
        
    case Mode_Track:

        HAL_TIM_Base_Stop_IT(&htim3);
        htim3.Init.Period = 20000/Speed_k-1;
        if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
        {
            Error_Handler();
        }

        Pixel_Point tem_Point;
        for (int i = 0; i < Track_Point.size; ++i)
        {
            tem_Point = Point_Queue_Dequeue(&Track_Point);
            Point_Queue_Enqueue(&(Track_Point), tem_Point); //不清空队列
            Point_Queue_Enqueue(&(sys_set.Target_Point), tem_Point);
        }
        Point_Queue_Double(&(sys_set.Target_Point));
        
        sys_set.Flag.Arrive = 1;
        HAL_TIM_Base_Start_IT(&htim3);
        break;
    }
    sys_set.Flag.End = 0;
    return (uint8_t)Current_Mode;
}


